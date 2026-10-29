#include "MyHeatMqtt.h"
#include <MyHeatWifi.h>
#include <MyHeatAlerts.h>
#include <mutex>

namespace MyHeatMqtt
{
    // Discovery payloads with the full device block do not fit PubSubClient's default buffer
    static constexpr uint16_t MIN_BUFFER_SIZE = 2048;
    static const char *HA_STATUS_TOPIC = "homeassistant/status";
    // Alerts raised while MQTT is offline (e.g. during boot) are kept and sent after connecting
    static constexpr size_t MAX_PENDING_ALERT_EVENTS = 10;

    // Alerts can be raised from the async web task as well as the main loop, so the handler only queues
    // and tick() publishes
    static std::vector<String> pendingAlertEvents;
    static std::mutex pendingAlertEventsMutex;

    // Hash of the last published state per entity. States are built in stack buffers and compared by hash,
    // so the change check allocates nothing on the heap
    using MyHeatUtils::ChangeTracker;
    ChangeTracker temperatureStatusTracker;
    ChangeTracker temperatureValueTracker;
    ChangeTracker relayModeTracker;
    ChangeTracker relayActiveTracker;
    ChangeTracker functionEnabledTracker;
    ChangeTracker functionNameTracker;
    ChangeTracker smokeTracker;
    enum SmokeSlot : byte { SMOKE_STATUS = 0, SMOKE_ALARM = 1 };

    // Smoke level change (in raw ADC counts, 0-4095) that is pushed immediately, smaller jitter waits for the interval
    static constexpr int SMOKE_VALUE_DEADBAND = 20;
    static int lastPublishedSmokeValue = -1;

    void resetTrackers()
    {
        temperatureStatusTracker.reset();
        temperatureValueTracker.reset();
        relayModeTracker.reset();
        relayActiveTracker.reset();
        functionEnabledTracker.reset();
        functionNameTracker.reset();
        smokeTracker.reset();
        lastPublishedSmokeValue = -1;
    }

    WiFiClient espClient;
    PubSubClient mqtt(espClient);
    MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
    MyHeatSave *mqttData = nullptr;
    MqttSettings settings;

    String chipId;
    String baseTopic;
    String clientId;
    String availTopic;

    uint32_t lastPublishTick = 0;
    uint32_t lastStateRevision = 0;
    uint32_t lastConnectAttempt = 0;
    bool wasConnected = false;
    bool isRediscoveryRequested = false;
    bool isRestartRequested = false;

    byte lastTemperatureCount = 0;
    byte lastRelayCount = 0;
    byte lastFunctionCount = 0;

    void MqttSettings::serialize(JsonDocument &json)
    {
        json["broker"] = broker;
        json["port"] = port;
        json["user"] = user;
        json["password"] = password;
        json["isEnabled"] = isEnabled;
        json["publishInterval"] = publishInterval;
        json["deviceName"] = deviceName;
    }

    void MqttSettings::deserialize(const JsonDocument &json)
    {
        broker = json["broker"] | MQTT_BROKER;
        port = json["port"] | MQTT_PORT;
        user = json["user"] | MQTT_USER;
        password = json["password"] | MQTT_PASSWORD;
        isEnabled = json["isEnabled"] | MQTT_ENABLED;
        publishInterval = json["publishInterval"] | MQTT_PUBLISH_INTERVAL;
        deviceName = json["deviceName"] | MQTT_DEVICE_NAME;
    }

    void MqttSettings::manualDeserialize(const JsonDocument& json)
    {
        deserialize(json);
    }

    void begin()
    {
        chipId = String((uint32_t)(ESP.getEfuseMac() >> 32), HEX) + String((uint32_t)ESP.getEfuseMac(), HEX);
        baseTopic = "myheat/" + chipId;
        clientId = "myheat_" + chipId;
        availTopic = baseTopic + "/status";

        mqttData = new MyHeatSave("/mqtt.json", &settings);
        mqttData->read();

        mqtt.setServer(settings.broker.c_str(), settings.port);
        mqtt.setBufferSize(max(MQTT_MAX_PACKET_SIZE, (int)MIN_BUFFER_SIZE));
        mqtt.setCallback(onMessage);
        mqtt.setKeepAlive(60);

        lastTemperatureCount = myHeatDevice.temperatures.getTemperatureCount();
        lastRelayCount = myHeatDevice.relays.getRelayCount();
        lastFunctionCount = myHeatDevice.customFunctions.getCustomFunctionCount();

        MyHeatAlerts::registerEventHandler(onAlertEvent);
    }

    void connect()
    {
        if (mqtt.connect(
                clientId.c_str(),
                settings.user.length() > 0 ? settings.user.c_str() : nullptr,
                settings.password.length() > 0 ? settings.password.c_str() : nullptr,
                availTopic.c_str(),
                1,
                true,
                "offline"))
        {
            // Birth message
            mqtt.publish(availTopic.c_str(), "online", true);

            resetTrackers();
            publishAllDiscoveryConfigs();
            subscribeToCommands();
            publishAllStates(true);
            publishDiagnostics();

            lastPublishTick = millis();
            wasConnected = true;
        }
    }

    void disconnect()
    {
        if (mqtt.connected())
        {
            mqtt.publish(availTopic.c_str(), "offline", true);
            mqtt.disconnect();
        }
    }

    bool isConnected()
    {
        return mqtt.connected();
    }

    void restart()
    {
        // Explicit offline: a clean disconnect does not fire the last will
        disconnect();
        delay(100);
        ESP.restart();
    }

    void tick()
    {
        if (!settings.isEnabled || !MyHeatWifi::getInstance().isConnected())
        {
            if (wasConnected)
            {
                disconnect();
                wasConnected = false;
            }
            return;
        }

        // Prevent LwIP sys_untimeout crash by waiting for SNTP DNS to finish.
        // We know SNTP is finished when time is synced.
        // If there's no internet, time will never sync, so we allow MQTT to connect
        // after 15 seconds to ensure local HA works without internet!
        if (!MyHeatWifi::getInstance().isTimeSynced())
        {
            if (millis() < 15000) return;
        }

        if (!mqtt.connected())
        {
            if (millis() - lastConnectAttempt > 5000)
            {
                lastConnectAttempt = millis();
                if (settings.broker.length() > 0)
                {
                    connect();
                }
            }
            return;
        }

        mqtt.loop();
        publishPendingAlertEvents();

        if (isRestartRequested)
        {
            restart();
            return;
        }

        if (myHeatDevice.temperatures.getTemperatureCount() != lastTemperatureCount ||
            myHeatDevice.relays.getRelayCount() != lastRelayCount ||
            myHeatDevice.customFunctions.getCustomFunctionCount() != lastFunctionCount)
        {
            handleEntityCountChange();
            publishAllStates(true);
        }

        // Home Assistant restarted, it may have lost entities that are not retained on the broker
        if (isRediscoveryRequested)
        {
            isRediscoveryRequested = false;
            publishAllDiscoveryConfigs();
            publishAllStates(true);
            publishDiagnostics();
        }

        if (millis() - lastPublishTick > settings.publishInterval)
        {
            lastPublishTick = millis();
            publishAllStates(true);
            publishDiagnostics();
        }
        else if (myHeatDevice.getStateRevision() != lastStateRevision)
        {
            // Nothing is built or compared until the device has refreshed its state
            lastStateRevision = myHeatDevice.getStateRevision();
            publishFunctionNameChanges();
            publishAllStates(false);
        }
    }

    // ----- Discovery -----

    void publishAllDiscoveryConfigs()
    {
        for (byte i = 0; i < myHeatDevice.temperatures.getTemperatureCount(); i++)
        {
            publishTemperatureDiscovery(i);
            delay(20);
        }

        for (byte i = 0; i < myHeatDevice.relays.getRelayCount(); i++)
        {
            publishRelaySwitchDiscovery(i);
            delay(20);
            publishRelayModeDiscovery(i);
            delay(20);
            publishRelayActiveDiscovery(i);
            delay(20);
        }

        for (byte i = 0; i < myHeatDevice.customFunctions.getCustomFunctionCount(); i++)
        {
            publishFunctionDiscovery(i);
            delay(20);
        }

        publishSmokeDiscovery();
        delay(20);
        publishDiagnosticsDiscovery();
        publishRestartDiscovery();
        publishAlertsDiscovery();
    }

    void addDeviceBlock(JsonDocument &doc)
    {
        JsonObject device = doc["device"].to<JsonObject>();
        JsonArray identifiers = device["identifiers"].to<JsonArray>();
        identifiers.add(clientId);
        JsonArray mac = device["connections"].to<JsonArray>().add<JsonArray>();
        mac.add("mac");
        mac.add(WiFi.macAddress());
        device["name"] = settings.deviceName.length() > 0 ? settings.deviceName : String(MQTT_DEVICE_NAME);
        device["manufacturer"] = "MyHeat";
        device["model"] = "MyHeat A1";
        device["sw_version"] = FIRMWARE_VERSION;
        device["hw_version"] = String(ESP.getChipModel()) + " rev " + String(ESP.getChipRevision());
        device["configuration_url"] = "http://" + WiFi.localIP().toString();
    }

    void addAvailability(JsonDocument &doc, const String& extraAvailTopic)
    {
        if (extraAvailTopic.length() > 0) {
            JsonArray avail = doc["availability"].to<JsonArray>();
            JsonObject devAvail = avail.add<JsonObject>();
            devAvail["topic"] = "~/status";
            devAvail["payload_available"] = "online";
            devAvail["payload_not_available"] = "offline";

            JsonObject extraAvail = avail.add<JsonObject>();
            extraAvail["topic"] = extraAvailTopic;
            extraAvail["payload_available"] = "online";
            extraAvail["payload_not_available"] = "offline";

            doc["availability_mode"] = "all";
        } else {
            doc["availability_topic"] = "~/status";
            doc["payload_available"] = "online";
            doc["payload_not_available"] = "offline";
        }
    }

    JsonDocument createDiscoveryDoc(const String &component, const String &objectId, const String &name)
    {
        JsonDocument doc;
        doc["~"] = baseTopic;
        doc["name"] = name;
        doc["unique_id"] = clientId + "_" + objectId;
        // Replaces the deprecated object_id, HA expects the full entity id including the domain
        doc["default_entity_id"] = component + "." + clientId + "_" + objectId;
        addDeviceBlock(doc);
        return doc;
    }

    void publishDiscovery(const String &component, const String &objectId, JsonDocument &doc)
    {
        String topic = "homeassistant/" + component + "/" + clientId + "/" + objectId + "/config";
        String payload;
        serializeJson(doc, payload);
        if (!mqtt.publish(topic.c_str(), payload.c_str(), true))
        {
            Serial.println("MQTT discovery publish failed: " + topic);
        }
    }

    void publishTemperatureDiscovery(byte index)
    {
        String objectId = "temperature_" + String(index);
        JsonDocument doc = createDiscoveryDoc("sensor", objectId, "Temperature " + String(index));
        doc["device_class"] = "temperature";
        doc["state_class"] = "measurement";
        doc["unit_of_measurement"] = "°C";
        doc["state_topic"] = "~/temperature/" + String(index);
        doc["suggested_display_precision"] = 1;
        addAvailability(doc, "~/temperature/" + String(index) + "/status");
        publishDiscovery("sensor", objectId, doc);
    }

    void publishRelaySwitchDiscovery(byte index)
    {
        // Shows the real relay output and overrides it: toggling sets the mode to on/off, auto is restored via the mode select
        String objectId = "relay_" + String(index);
        JsonDocument doc = createDiscoveryDoc("switch", objectId, "Relay " + String(index));
        doc["device_class"] = "switch";
        doc["state_topic"] = "~/relay/" + String(index) + "/active";
        doc["command_topic"] = "~/relay/" + String(index) + "/set";
        doc["payload_on"] = "ON";
        doc["payload_off"] = "OFF";
        addAvailability(doc);
        publishDiscovery("switch", objectId, doc);
    }

    void publishRelayModeDiscovery(byte index)
    {
        String objectId = "relay_" + String(index) + "_mode";
        JsonDocument doc = createDiscoveryDoc("select", objectId, "Relay " + String(index) + " Mode");
        doc["icon"] = "mdi:cog";
        doc["state_topic"] = "~/relay/" + String(index) + "/mode";
        doc["command_topic"] = "~/relay/" + String(index) + "/mode/set";
        JsonArray options = doc["options"].to<JsonArray>();
        options.add("off");
        options.add("on");
        options.add("auto");
        addAvailability(doc);
        publishDiscovery("select", objectId, doc);
    }

    void publishRelayActiveDiscovery(byte index)
    {
        String objectId = "relay_" + String(index) + "_active";
        JsonDocument doc = createDiscoveryDoc("binary_sensor", objectId, "Relay " + String(index) + " Active");
        doc["device_class"] = "running";
        doc["state_topic"] = "~/relay/" + String(index) + "/active";
        doc["payload_on"] = "ON";
        doc["payload_off"] = "OFF";
        addAvailability(doc);
        publishDiscovery("binary_sensor", objectId, doc);
    }

    String getFunctionName(byte index)
    {
        String name = myHeatDevice.customFunctions.getCustomFunction(index).getName();
        return name.length() > 0 ? name : "Function " + String(index);
    }

    void publishFunctionDiscovery(byte index)
    {
        String name = getFunctionName(index);
        String objectId = "function_" + String(index);
        JsonDocument doc = createDiscoveryDoc("switch", objectId, name);
        doc["icon"] = "mdi:function";
        doc["state_topic"] = "~/function/" + String(index) + "/enabled";
        doc["command_topic"] = "~/function/" + String(index) + "/enabled/set";
        doc["payload_on"] = "ON";
        doc["payload_off"] = "OFF";
        addAvailability(doc);
        publishDiscovery("switch", objectId, doc);

        functionNameTracker.update(index, MyHeatUtils::hashString(myHeatDevice.customFunctions.getCustomFunction(index).getName()));
    }

    void publishFunctionNameChanges()
    {
        for (byte i = 0; i < myHeatDevice.customFunctions.getCustomFunctionCount(); i++)
        {
            uint32_t nameHash = MyHeatUtils::hashString(myHeatDevice.customFunctions.getCustomFunction(i).getName());
            if (functionNameTracker.update(i, nameHash))
            {
                publishFunctionDiscovery(i);
            }
        }
    }

    void publishSmokeDiscovery()
    {
        // Configs left by older firmware versions
        clearDiscoveryConfig("binary_sensor", "smoke");
        clearDiscoveryConfig("sensor", "smoke_value");

        JsonDocument alarmDoc = createDiscoveryDoc("binary_sensor", "smoke_alarm", "Smoke");
        alarmDoc["device_class"] = "smoke";
        alarmDoc["state_topic"] = "~/smoke/alarm";
        alarmDoc["payload_on"] = "ON";
        alarmDoc["payload_off"] = "OFF";
        addAvailability(alarmDoc, "~/smoke/status");
        publishDiscovery("binary_sensor", "smoke_alarm", alarmDoc);

        // Raw MQ-2 reading, kept on the original unique_id so existing history is preserved
        JsonDocument valueDoc = createDiscoveryDoc("sensor", "smoke", "Smoke Level");
        valueDoc["icon"] = "mdi:smoke-detector-variant";
        valueDoc["state_topic"] = "~/smoke/value";
        valueDoc["state_class"] = "measurement";
        addAvailability(valueDoc, "~/smoke/status");
        publishDiscovery("sensor", "smoke", valueDoc);
    }

    JsonDocument createDiagnosticDoc(const String &objectId, const String &name, const String &valueKey)
    {
        JsonDocument doc = createDiscoveryDoc("sensor", objectId, name);
        doc["state_topic"] = "~/diagnostics";
        doc["value_template"] = "{{ value_json." + valueKey + " }}";
        doc["entity_category"] = "diagnostic";
        addAvailability(doc);
        return doc;
    }

    void publishDiagnosticsDiscovery()
    {
        JsonDocument rssiDoc = createDiagnosticDoc("wifi_signal", "WiFi Signal", "rssi");
        rssiDoc["device_class"] = "signal_strength";
        rssiDoc["unit_of_measurement"] = "dBm";
        rssiDoc["state_class"] = "measurement";
        publishDiscovery("sensor", "wifi_signal", rssiDoc);
        delay(20);

        JsonDocument ipDoc = createDiagnosticDoc("ip_address", "IP Address", "ip");
        ipDoc["icon"] = "mdi:ip-network";
        publishDiscovery("sensor", "ip_address", ipDoc);
        delay(20);

        // Replaced by Last Boot: an uptime counter changes on every publish and grows the HA database
        clearDiscoveryConfig("sensor", "uptime");

        JsonDocument bootDoc = createDiagnosticDoc("last_boot", "Last Boot", "boot");
        bootDoc["device_class"] = "timestamp";
        publishDiscovery("sensor", "last_boot", bootDoc);
        delay(20);

        JsonDocument heapDoc = createDiagnosticDoc("free_heap", "Free Memory", "heap");
        heapDoc["device_class"] = "data_size";
        heapDoc["unit_of_measurement"] = "B";
        heapDoc["state_class"] = "measurement";
        publishDiscovery("sensor", "free_heap", heapDoc);
        delay(20);

        // Free memory alone does not show fragmentation: a shrinking largest block with flat free memory does
        JsonDocument blockDoc = createDiagnosticDoc("largest_free_block", "Largest Free Block", "heap_block");
        blockDoc["device_class"] = "data_size";
        blockDoc["unit_of_measurement"] = "B";
        blockDoc["state_class"] = "measurement";
        publishDiscovery("sensor", "largest_free_block", blockDoc);
        delay(20);

        JsonDocument minHeapDoc = createDiagnosticDoc("min_free_heap", "Min Free Memory", "heap_min");
        minHeapDoc["device_class"] = "data_size";
        minHeapDoc["unit_of_measurement"] = "B";
        minHeapDoc["state_class"] = "measurement";
        publishDiscovery("sensor", "min_free_heap", minHeapDoc);
        delay(20);

        JsonDocument ntpDoc = createDiagnosticDoc("last_time_sync", "Last Time Sync", "ntp_sync");
        ntpDoc["device_class"] = "timestamp";
        publishDiscovery("sensor", "last_time_sync", ntpDoc);
        delay(20);
    }

    void publishRestartDiscovery()
    {
        JsonDocument doc = createDiscoveryDoc("button", "restart", "Restart");
        doc["device_class"] = "restart";
        doc["entity_category"] = "config";
        doc["command_topic"] = "~/restart/set";
        doc["payload_press"] = "PRESS";
        addAvailability(doc);
        publishDiscovery("button", "restart", doc);
    }

    void publishAlertsDiscovery()
    {
        JsonDocument doc = createDiscoveryDoc("event", "alerts", "Alerts");
        doc["icon"] = "mdi:alert-circle-outline";
        doc["state_topic"] = "~/alert";

        // Every alert has a raised and a "_resolved" event type
        JsonArray eventTypes = doc["event_types"].to<JsonArray>();
        for (byte i = 0; i < static_cast<byte>(AlertType::COUNT); i++)
        {
            String typeName = MyHeatAlerts::getAlertTypeName(static_cast<AlertType>(i));
            eventTypes.add(typeName);
            eventTypes.add(typeName + "_resolved");
        }

        addAvailability(doc);
        publishDiscovery("event", "alerts", doc);
    }

    void clearDiscoveryConfig(const String &component, const String &objectId)
    {
        String topic = "homeassistant/" + component + "/" + clientId + "/" + objectId + "/config";
        mqtt.publish(topic.c_str(), "", true);
    }

    // ----- State publishing -----

    void publishState(ChangeTracker &tracker, size_t slot, const char *topic, const char *payload, bool force)
    {
        bool isChanged = tracker.update(slot, MyHeatUtils::hashString(payload));
        if (!isChanged && !force)
        {
            return;
        }

        if (!mqtt.publish(topic, payload, true))
        {
            tracker.invalidate(slot); // Retry on the next check
        }
    }

    void publishAllStates(bool force)
    {
        for (byte i = 0; i < myHeatDevice.temperatures.getTemperatureCount(); i++)
        {
            publishTemperatureState(i, force);
        }

        for (byte i = 0; i < myHeatDevice.relays.getRelayCount(); i++)
        {
            publishRelayModeState(i, force);
            publishRelayActiveState(i, force);
        }

        for (byte i = 0; i < myHeatDevice.customFunctions.getCustomFunctionCount(); i++)
        {
            publishFunctionState(i, force);
        }

        publishSmokeState(force);
    }

    void publishTemperatureState(byte index, bool force)
    {
        char topic[96];
        float temp = myHeatDevice.temperatures.getTemperature(index);
        bool isAvailable = temp > -127.0;

        snprintf(topic, sizeof(topic), "%s/temperature/%u/status", baseTopic.c_str(), index);
        publishState(temperatureStatusTracker, index, topic, isAvailable ? "online" : "offline", force);

        if (isAvailable)
        {
            char payload[16];
            snprintf(payload, sizeof(payload), "%.1f", temp);
            snprintf(topic, sizeof(topic), "%s/temperature/%u", baseTopic.c_str(), index);
            publishState(temperatureValueTracker, index, topic, payload, force);
        }
    }

    void publishRelayModeState(byte index, bool force)
    {
        static const char *MODE_NAMES[] = {"off", "on", "auto"};
        byte mode = static_cast<byte>(myHeatDevice.relays.getRelay(index).getMode());

        char topic[96];
        snprintf(topic, sizeof(topic), "%s/relay/%u/mode", baseTopic.c_str(), index);
        publishState(relayModeTracker, index, topic, MODE_NAMES[mode < 3 ? mode : 0], force);
    }

    void publishRelayActiveState(byte index, bool force)
    {
        bool active = myHeatDevice.relays.getRelay(index).getIsActive();

        char topic[96];
        snprintf(topic, sizeof(topic), "%s/relay/%u/active", baseTopic.c_str(), index);
        publishState(relayActiveTracker, index, topic, active ? "ON" : "OFF", force);
    }

    void publishFunctionState(byte index, bool force)
    {
        bool enabled = myHeatDevice.customFunctions.getCustomFunction(index).getIsEnabled();

        char topic[96];
        snprintf(topic, sizeof(topic), "%s/function/%u/enabled", baseTopic.c_str(), index);
        publishState(functionEnabledTracker, index, topic, enabled ? "ON" : "OFF", force);
    }

    void publishSmokeState(bool force)
    {
        char topic[96];
        int val = myHeatDevice.smokeSensor.getValue();
        bool isAvailable = myHeatDevice.smokeSensor.getIsEnabled() && val != -1;

        snprintf(topic, sizeof(topic), "%s/smoke/status", baseTopic.c_str());
        publishState(smokeTracker, SMOKE_STATUS, topic, isAvailable ? "online" : "offline", force);

        if (!isAvailable)
        {
            return;
        }

        bool active = myHeatDevice.smokeSensor.getSmokeSensorAlert() == SSA_OVER_THRESHOLD;
        snprintf(topic, sizeof(topic), "%s/smoke/alarm", baseTopic.c_str());
        publishState(smokeTracker, SMOKE_ALARM, topic, active ? "ON" : "OFF", force);

        // The raw analog value jitters by a few counts, so only changes bigger than the deadband are pushed
        if (force || lastPublishedSmokeValue < 0 || abs(val - lastPublishedSmokeValue) >= SMOKE_VALUE_DEADBAND)
        {
            char payload[12];
            snprintf(payload, sizeof(payload), "%d", val);
            snprintf(topic, sizeof(topic), "%s/smoke/value", baseTopic.c_str());
            if (mqtt.publish(topic, payload, true))
            {
                lastPublishedSmokeValue = val;
            }
        }
    }

    // Writes a quoted ISO 8601 UTC timestamp, or JSON null (shown as unknown in HA) when time is not known
    void formatJsonTimestamp(time_t timestamp, char *buffer, size_t size)
    {
        if (timestamp <= 0)
        {
            strlcpy(buffer, "null", size);
            return;
        }

        struct tm timeinfo;
        gmtime_r(&timestamp, &timeinfo);
        strftime(buffer, size, "\"%Y-%m-%dT%H:%M:%SZ\"", &timeinfo);
    }

    void publishDiagnostics()
    {
        // Computed once after the clock is synced, recomputing would jitter by a second and create a new HA state each time
        static time_t bootTime = 0;
        if (bootTime == 0 && MyHeatWifi::getInstance().isTimeSynced())
        {
            bootTime = time(nullptr) - (time_t)(esp_timer_get_time() / 1000000ULL);
        }

        char ntpSync[24];
        char boot[24];
        formatJsonTimestamp(MyHeatWifi::getInstance().getLastNtpSyncTime(), ntpSync, sizeof(ntpSync));
        formatJsonTimestamp(bootTime, boot, sizeof(boot));

        IPAddress ip = WiFi.localIP();
        char payload[256];
        snprintf(payload, sizeof(payload),
                 "{\"rssi\":%d,\"ip\":\"%u.%u.%u.%u\",\"boot\":%s,\"heap\":%lu,\"heap_min\":%lu,\"heap_block\":%lu,\"ntp_sync\":%s}",
                 WiFi.RSSI(), ip[0], ip[1], ip[2], ip[3],
                 boot,
                 (unsigned long)ESP.getFreeHeap(),
                 (unsigned long)ESP.getMinFreeHeap(),
                 (unsigned long)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT),
                 ntpSync);

        char topic[96];
        snprintf(topic, sizeof(topic), "%s/diagnostics", baseTopic.c_str());
        mqtt.publish(topic, payload, true);
    }

    // ----- Alerts -----

    void onAlertEvent(const AlertEvent &event)
    {
        if (!settings.isEnabled)
        {
            return;
        }

        JsonDocument doc;
        String eventType = MyHeatAlerts::getAlertTypeName(event.type);
        doc["event_type"] = event.isResolved ? eventType + "_resolved" : eventType;
        if (event.index >= 0)
        {
            doc["index"] = event.index;
        }
        doc["message"] = event.message;

        String payload;
        serializeJson(doc, payload);

        std::lock_guard<std::mutex> lock(pendingAlertEventsMutex);
        if (pendingAlertEvents.size() >= MAX_PENDING_ALERT_EVENTS)
        {
            pendingAlertEvents.erase(pendingAlertEvents.begin()); // Keep the newest
        }
        pendingAlertEvents.push_back(payload);
    }

    void publishPendingAlertEvents()
    {
        std::vector<String> events;
        {
            std::lock_guard<std::mutex> lock(pendingAlertEventsMutex);
            events.swap(pendingAlertEvents);
        }

        if (events.empty())
        {
            return;
        }

        char topic[96];
        snprintf(topic, sizeof(topic), "%s/alert", baseTopic.c_str());
        for (const String &payload : events)
        {
            // Not retained: HA would replay the last alert on every restart
            mqtt.publish(topic, payload.c_str(), false);
        }
    }

    // ----- Command handling -----

    int extractIndex(const String &topicStr, const String &prefix, const String &suffix)
    {
        int start = topicStr.indexOf(prefix) + prefix.length();
        int end = topicStr.indexOf(suffix);
        if (start > 0 && end > start)
        {
            return topicStr.substring(start, end).toInt();
        }
        return -1;
    }

    void onMessage(char *topic, byte *payload, unsigned int length)
    {
        String topicStr(topic);
        String value;
        for (unsigned int i = 0; i < length; i++)
            value += (char)payload[i];

        if (topicStr == HA_STATUS_TOPIC)
        {
            // Handled in tick(): publishing a burst of configs from inside the callback reuses the receive buffer
            if (value == "online")
                isRediscoveryRequested = true;
        }
        else if (topicStr == baseTopic + "/restart/set")
        {
            if (value == "PRESS")
                isRestartRequested = true;
        }
        else if (topicStr.startsWith(baseTopic + "/relay/") && topicStr.endsWith("/mode/set"))
        {
            int index = extractIndex(topicStr, "/relay/", "/mode/set");
            if (index >= 0)
            {
                handleRelayModeCommand(index, value);
            }
        }
        else if (topicStr.startsWith(baseTopic + "/relay/") && topicStr.endsWith("/set"))
        {
            int index = extractIndex(topicStr, "/relay/", "/set");
            if (index >= 0)
            {
                handleRelaySwitchCommand(index, value);
            }
        }
        else if (topicStr.startsWith(baseTopic + "/function/") && topicStr.endsWith("/enabled/set"))
        {
            int index = extractIndex(topicStr, "/function/", "/enabled/set");
            if (index >= 0)
            {
                handleFunctionEnabledCommand(index, value);
            }
        }
    }

    void handleRelayModeCommand(byte index, const String &value)
    {
        if (index >= myHeatDevice.relays.getRelayCount())
            return;

        if (value == "off")
            applyRelayMode(index, RelayMode::OFF);
        else if (value == "on")
            applyRelayMode(index, RelayMode::ON);
        else if (value == "auto")
            applyRelayMode(index, RelayMode::AUTO);
        // Unknown payload must not switch heating off
    }

    void handleRelaySwitchCommand(byte index, const String &value)
    {
        if (index >= myHeatDevice.relays.getRelayCount())
            return;

        if (value == "ON")
            applyRelayMode(index, RelayMode::ON);
        else if (value == "OFF")
            applyRelayMode(index, RelayMode::OFF);
    }

    void applyRelayMode(byte index, RelayMode mode)
    {
        myHeatDevice.relays.setRelayMode(index, mode);
        // Apply right away so the published active state is the real one
        myHeatDevice.manualTick();
        publishRelayModeState(index, true);
        publishRelayActiveState(index, true);
    }

    void handleFunctionEnabledCommand(byte index, const String &value)
    {
        if (index >= myHeatDevice.customFunctions.getCustomFunctionCount())
            return;

        if (value != "ON" && value != "OFF")
            return;

        myHeatDevice.customFunctions.setCustomFunctionIsEnabled(index, value == "ON");
        myHeatDevice.manualTick();
        publishFunctionState(index, true);
    }

    void subscribeToCommands()
    {
        String relayModeCommand = baseTopic + "/relay/+/mode/set";
        mqtt.subscribe(relayModeCommand.c_str());

        String relaySwitchCommand = baseTopic + "/relay/+/set";
        mqtt.subscribe(relaySwitchCommand.c_str());

        String functionCommand = baseTopic + "/function/+/enabled/set";
        mqtt.subscribe(functionCommand.c_str());

        String restartCommand = baseTopic + "/restart/set";
        mqtt.subscribe(restartCommand.c_str());

        mqtt.subscribe(HA_STATUS_TOPIC);
    }

    // ----- Settings -----

    String getBroker() { return settings.broker; }
    uint16_t getPort() { return settings.port; }
    String getUser() { return settings.user; }
    String getPassword() { return settings.password; }
    bool getIsEnabled() { return settings.isEnabled; }
    uint32_t getPublishInterval() { return settings.publishInterval; }
    String getDeviceName() { return settings.deviceName; }

    void setSettings(const String &broker, uint16_t port,
                     const String &user, const String &password,
                     bool isEnabled, uint32_t publishInterval,
                     const String &deviceName)
    {
        settings.broker = broker;
        settings.port = port;
        settings.user = user;
        settings.password = password;
        settings.isEnabled = isEnabled;
        settings.publishInterval = publishInterval;
        settings.deviceName = deviceName;

        mqtt.setServer(settings.broker.c_str(), settings.port);

        if (mqtt.connected()) {
            disconnect(); // will trigger reconnect on next tick with new settings
        }
    }

    void save()
    {
        mqttData->save();
    }

    void handleEntityCountChange()
    {
        for (byte i = myHeatDevice.temperatures.getTemperatureCount(); i < lastTemperatureCount; i++)
        {
            clearDiscoveryConfig("sensor", "temperature_" + String(i));
        }
        for (byte i = myHeatDevice.relays.getRelayCount(); i < lastRelayCount; i++)
        {
            clearDiscoveryConfig("select", "relay_" + String(i) + "_mode");
            clearDiscoveryConfig("binary_sensor", "relay_" + String(i) + "_active");
            clearDiscoveryConfig("switch", "relay_" + String(i));
        }
        for (byte i = myHeatDevice.customFunctions.getCustomFunctionCount(); i < lastFunctionCount; i++)
        {
            clearDiscoveryConfig("switch", "function_" + String(i));
        }

        publishAllDiscoveryConfigs();
        subscribeToCommands();

        lastTemperatureCount = myHeatDevice.temperatures.getTemperatureCount();
        lastRelayCount = myHeatDevice.relays.getRelayCount();
        lastFunctionCount = myHeatDevice.customFunctions.getCustomFunctionCount();
    }

}
