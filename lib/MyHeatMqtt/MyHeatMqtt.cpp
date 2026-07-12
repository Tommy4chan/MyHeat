#include "MyHeatMqtt.h"
#include <MyHeatWifi.h>

namespace MyHeatMqtt
{
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
    uint32_t lastConnectAttempt = 0;
    bool wasConnected = false;

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
    }

    void MqttSettings::deserialize(JsonDocument &json)
    {
        broker = json["broker"] | MQTT_BROKER;
        port = json["port"] | MQTT_PORT;
        user = json["user"] | MQTT_USER;
        password = json["password"] | MQTT_PASSWORD;
        isEnabled = json["isEnabled"] | MQTT_ENABLED;
        publishInterval = json["publishInterval"] | MQTT_PUBLISH_INTERVAL;
    }

    void MqttSettings::manualDeserialize(JsonDocument json)
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
        mqtt.setBufferSize(MQTT_MAX_PACKET_SIZE);
        mqtt.setCallback(onMessage);
        mqtt.setKeepAlive(60);

        lastTemperatureCount = myHeatDevice.temperatures.getTemperatureCount();
        lastRelayCount = myHeatDevice.relays.getRelayCount();
        lastFunctionCount = myHeatDevice.customFunctions.getCustomFunctionCount();
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

            publishAllDiscoveryConfigs();
            subscribeToCommands();
            publishAllStates();

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
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo, 0) || timeinfo.tm_year < 100)
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

        if (myHeatDevice.temperatures.getTemperatureCount() != lastTemperatureCount ||
            myHeatDevice.relays.getRelayCount() != lastRelayCount ||
            myHeatDevice.customFunctions.getCustomFunctionCount() != lastFunctionCount)
        {
            handleEntityCountChange();
        }

        if (millis() - lastPublishTick > settings.publishInterval)
        {
            lastPublishTick = millis();
            publishAllStates();
        }
    }

    void publishAllDiscoveryConfigs()
    {
        for (byte i = 0; i < myHeatDevice.temperatures.getTemperatureCount(); i++)
        {
            publishTemperatureDiscovery(i);
            delay(20);
        }

        for (byte i = 0; i < myHeatDevice.relays.getRelayCount(); i++)
        {
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
    }

    void addDeviceBlock(JsonDocument &doc)
    {
        JsonObject device = doc["device"].to<JsonObject>();
        JsonArray identifiers = device["identifiers"].to<JsonArray>();
        identifiers.add(clientId);
        device["name"] = "MyHeat Controller";
        device["manufacturer"] = "MyHeat";
        device["model"] = "MyHeat ESP32";
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

    void publishTemperatureDiscovery(byte index)
    {
        JsonDocument doc;
        doc["~"] = baseTopic;
        doc["name"] = "Temperature " + String(index + 1);
        doc["unique_id"] = clientId + "_temperature_" + String(index);
        doc["object_id"] = clientId + "_temperature_" + String(index);
        doc["device_class"] = "temperature";
        doc["state_class"] = "measurement";
        doc["unit_of_measurement"] = "°C";
        doc["state_topic"] = "~/temperature/" + String(index);
        doc["suggested_display_precision"] = 1;
        addAvailability(doc, "~/temperature/" + String(index) + "/status");
        addDeviceBlock(doc);

        String topic = "homeassistant/sensor/" + clientId + "/temperature_" + String(index) + "/config";
        String payload;
        serializeJson(doc, payload);
        mqtt.publish(topic.c_str(), payload.c_str(), true);
    }

    void publishRelayModeDiscovery(byte index)
    {
        JsonDocument doc;
        doc["~"] = baseTopic;
        doc["name"] = "Relay " + String(index + 1) + " Mode";
        doc["unique_id"] = clientId + "_relay_" + String(index) + "_mode";
        doc["object_id"] = clientId + "_relay_" + String(index) + "_mode";
        doc["icon"] = "mdi:electric-switch";
        doc["state_topic"] = "~/relay/" + String(index) + "/mode";
        doc["command_topic"] = "~/relay/" + String(index) + "/mode/set";
        JsonArray options = doc["options"].to<JsonArray>();
        options.add("off");
        options.add("on");
        options.add("auto");
        addAvailability(doc);
        addDeviceBlock(doc);

        String topic = "homeassistant/select/" + clientId + "/relay_" + String(index) + "_mode/config";
        String payload;
        serializeJson(doc, payload);
        mqtt.publish(topic.c_str(), payload.c_str(), true);
    }

    void publishRelayActiveDiscovery(byte index)
    {
        JsonDocument doc;
        doc["~"] = baseTopic;
        doc["name"] = "Relay " + String(index + 1) + " Active";
        doc["unique_id"] = clientId + "_relay_" + String(index) + "_active";
        doc["object_id"] = clientId + "_relay_" + String(index) + "_active";
        doc["device_class"] = "running";
        doc["state_topic"] = "~/relay/" + String(index) + "/active";
        doc["payload_on"] = "ON";
        doc["payload_off"] = "OFF";
        addAvailability(doc);
        addDeviceBlock(doc);

        String topic = "homeassistant/binary_sensor/" + clientId + "/relay_" + String(index) + "_active/config";
        String payload;
        serializeJson(doc, payload);
        mqtt.publish(topic.c_str(), payload.c_str(), true);
    }

    void publishFunctionDiscovery(byte index)
    {
        JsonDocument doc;
        doc["~"] = baseTopic;
        doc["name"] = "Function " + String(index + 1);
        doc["unique_id"] = clientId + "_function_" + String(index);
        doc["object_id"] = clientId + "_function_" + String(index);
        doc["icon"] = "mdi:function";
        doc["state_topic"] = "~/function/" + String(index) + "/enabled";
        doc["command_topic"] = "~/function/" + String(index) + "/enabled/set";
        doc["payload_on"] = "ON";
        doc["payload_off"] = "OFF";
        addAvailability(doc);
        addDeviceBlock(doc);

        String topic = "homeassistant/switch/" + clientId + "/function_" + String(index) + "/config";
        String payload;
        serializeJson(doc, payload);
        mqtt.publish(topic.c_str(), payload.c_str(), true);
    }

    void publishSmokeDiscovery()
    {
        clearDiscoveryConfig("binary_sensor", "smoke");
        clearDiscoveryConfig("sensor", "smoke_value");

        JsonDocument doc;
        doc["~"] = baseTopic;
        doc["name"] = "Smoke Detector";
        doc["unique_id"] = clientId + "_smoke";
        doc["object_id"] = clientId + "_smoke";
        doc["icon"] = "mdi:smoke-detector-variant";
        doc["state_topic"] = "~/smoke/state";
        doc["value_template"] = "{{ value_json.value }}";
        doc["json_attributes_topic"] = "~/smoke/state";
        doc["state_class"] = "measurement";
        addAvailability(doc, "~/smoke/status");
        addDeviceBlock(doc);

        String topic = "homeassistant/sensor/" + clientId + "/smoke/config";
        String payload;
        serializeJson(doc, payload);
        mqtt.publish(topic.c_str(), payload.c_str(), true);
    }

    void clearDiscoveryConfig(const String &component, const String &objectId)
    {
        String topic = "homeassistant/" + component + "/" + clientId + "/" + objectId + "/config";
        mqtt.publish(topic.c_str(), "", true);
    }

    void publishAllStates()
    {
        for (byte i = 0; i < myHeatDevice.temperatures.getTemperatureCount(); i++)
        {
            publishTemperatureState(i);
        }

        for (byte i = 0; i < myHeatDevice.relays.getRelayCount(); i++)
        {
            publishRelayModeState(i);
            publishRelayActiveState(i);
        }

        for (byte i = 0; i < myHeatDevice.customFunctions.getCustomFunctionCount(); i++)
        {
            publishFunctionState(i);
        }

        publishSmokeState();
    }

    void publishTemperatureState(byte index)
    {
        float temp = myHeatDevice.temperatures.getTemperature(index);
        String stateTopic = baseTopic + "/temperature/" + String(index);
        String availTopicLocal = baseTopic + "/temperature/" + String(index) + "/status";

        if (temp <= -127.0) {
            mqtt.publish(availTopicLocal.c_str(), "offline", true);
        } else {
            mqtt.publish(availTopicLocal.c_str(), "online", true);
            mqtt.publish(stateTopic.c_str(), String(temp, 1).c_str(), true);
        }
    }

    void publishRelayModeState(byte index)
    {
        byte mode = static_cast<byte>(myHeatDevice.relays.getRelay(index).getMode());
        String modeStr = "off";
        if (mode == 1)
            modeStr = "on";
        else if (mode == 2)
            modeStr = "auto";

        String topic = baseTopic + "/relay/" + String(index) + "/mode";
        mqtt.publish(topic.c_str(), modeStr.c_str(), true);
    }

    void publishRelayActiveState(byte index)
    {
        bool active = myHeatDevice.relays.getRelay(index).getIsActive();
        String topic = baseTopic + "/relay/" + String(index) + "/active";
        mqtt.publish(topic.c_str(), active ? "ON" : "OFF", true);
    }

    void publishFunctionState(byte index)
    {
        bool enabled = myHeatDevice.customFunctions.getCustomFunction(index).getIsEnabled();
        String topic = baseTopic + "/function/" + String(index) + "/enabled";
        mqtt.publish(topic.c_str(), enabled ? "ON" : "OFF", true);
    }

    void publishSmokeState()
    {
        bool enabled = myHeatDevice.smokeSensor.getIsEnabled();
        String topic = baseTopic + "/smoke/state";
        String availTopicLocal = baseTopic + "/smoke/status";
        
        if (!enabled) {
            mqtt.publish(availTopicLocal.c_str(), "offline", true);
            return;
        }

        int val = myHeatDevice.smokeSensor.getValue();
        if (val == -1) {
            mqtt.publish(availTopicLocal.c_str(), "offline", true);
            return;
        }

        mqtt.publish(availTopicLocal.c_str(), "online", true);
        bool active = myHeatDevice.smokeSensor.getSmokeSensorAlert() == SSA_OVER_THRESHOLD;
        
        JsonDocument doc;
        doc["value"] = val;
        doc["alarm"] = active ? "ON" : "OFF";
        
        String payload;
        serializeJson(doc, payload);
        mqtt.publish(topic.c_str(), payload.c_str(), true);
    }

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

        if (topicStr.startsWith(baseTopic + "/relay/") && topicStr.endsWith("/mode/set"))
        {
            int index = extractIndex(topicStr, "/relay/", "/mode/set");
            if (index >= 0)
            {
                handleRelayModeCommand(index, value);
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
        RelayMode mode = RelayMode::OFF;
        if (value == "on")
            mode = RelayMode::ON;
        else if (value == "auto")
            mode = RelayMode::AUTO;
        myHeatDevice.relays.setRelayMode(index, mode);
        publishRelayModeState(index);
        publishRelayActiveState(index);
    }

    void handleFunctionEnabledCommand(byte index, const String &value)
    {
        if (index >= myHeatDevice.customFunctions.getCustomFunctionCount())
            return;
        bool enabled = (value == "ON");
        myHeatDevice.customFunctions.setCustomFunctionIsEnabled(index, enabled);
        publishFunctionState(index);
    }

    void subscribeToCommands()
    {
        String relayCommand = baseTopic + "/relay/+/mode/set";
        mqtt.subscribe(relayCommand.c_str());

        String functionCommand = baseTopic + "/function/+/enabled/set";
        mqtt.subscribe(functionCommand.c_str());
    }

    String getBroker() { return settings.broker; }
    uint16_t getPort() { return settings.port; }
    String getUser() { return settings.user; }
    String getPassword() { return settings.password; }
    bool getIsEnabled() { return settings.isEnabled; }
    uint32_t getPublishInterval() { return settings.publishInterval; }

    void setSettings(const String &broker, uint16_t port,
                     const String &user, const String &password,
                     bool isEnabled, uint32_t publishInterval)
    {
        settings.broker = broker;
        settings.port = port;
        settings.user = user;
        settings.password = password;
        settings.isEnabled = isEnabled;
        settings.publishInterval = publishInterval;
        
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
