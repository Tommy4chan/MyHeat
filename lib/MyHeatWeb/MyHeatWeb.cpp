#include "MyHeatWeb.h"
#include <MyHeatAlerts.h>
#include <Update.h>

namespace MyHeatWeb
{
    static String getIndexEtag();

    void begin()
    {
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

        // index.html is cached but revalidated: the browser asks with its ETag and gets an empty 304 while the file
        // is the same. A plain long cache would keep an index pointing to deleted assets after a filesystem update.
        // The hashed assets keep the long cache since their names change with every build
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            String etag = getIndexEtag();
            if (etag.isEmpty()) {
                request->send(404);
                return;
            }

            AsyncWebServerResponse *response = request->header("If-None-Match") == etag
                ? request->beginResponse(304)
                : request->beginResponse(LittleFS, "/index.html", "text/html");
            response->addHeader("Cache-Control", "no-cache");
            response->addHeader("ETag", etag);
            request->send(response);
        });
        server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setCacheControl("max-age=6000000");
        setupOTA();
        websocket.enable(true);
        setupWebsocket();
        server.begin();
        MyHeatAlerts::registerHandler(sendAlertNotification);
    }

    // FNV-1a of the file content, index.html is ~0.5 KB and its size stays the same between builds
    static String getIndexEtag()
    {
        File file = LittleFS.open("/index.html", "r");
        if (!file) {
            return String();
        }

        uint32_t hash = 2166136261u;
        uint8_t buffer[128];
        size_t count;
        while ((count = file.read(buffer, sizeof(buffer))) > 0) {
            for (size_t i = 0; i < count; i++) {
                hash = (hash ^ buffer[i]) * 16777619u;
            }
        }
        file.close();

        char etag[11];
        snprintf(etag, sizeof(etag), "\"%08lx\"", (unsigned long)hash);
        return String(etag);
    }

    // Set only once Update.end() accepted the image, so a request without a file part can't trigger a reboot
    static bool isUpdateSucceeded = false;

    void setupOTA()
    {
        server.on("/update", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
            request->send(200);
        });

        server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
            MyHeatUtils::clearFlashUpdateActivity();
            bool shouldReboot = isUpdateSucceeded;
            isUpdateSucceeded = false;
            AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot ? String("OK") : String("FAIL: ") + Update.errorString());
            response->addHeader("Connection", "close");
            request->send(response);

            if (shouldReboot) {
                MyHeatWeb::restartRequired = true;
            }
        }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
            MyHeatUtils::markFlashUpdateActivity();

            if (!index) {
                isUpdateSucceeded = false;
                Serial.printf("Update Start: %s\n", filename.c_str());
                int cmd = U_FLASH;
                if (request->hasParam("type") && request->getParam("type")->value() == "fs") {
                    cmd = U_SPIFFS;
                }

                // An earlier upload that was cut off leaves Update running, every new begin() would fail until reboot
                if (Update.isRunning()) {
                    Update.abort();
                }

                // contentLength() is the whole multipart body, bigger than the file itself, so a full size
                // filesystem image never fit. The real size is set by end(true) from the bytes written
                if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
                    Update.printError(Serial);
                }
            }
            if (Update.isRunning() && !Update.hasError()) {
                if (Update.write(data, len) != len) {
                    Update.printError(Serial);
                }
            }
            if (final && Update.isRunning()) {
                if (Update.end(true)) {
                    isUpdateSucceeded = true;
                    Serial.printf("Update Success: %uB\n", index + len);
                } else {
                    Update.printError(Serial);
                }
            }
        });
    }

    void setupWebsocket()
    {
        websocket.onEvent(wsEventHandler);
        server.addHandler(&websocket);
    }

    void wsEventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
        if (type == WS_EVT_CONNECT)
        {
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            sendUsedPinsData();
            sendDeviceInfo();
            requestFullSend();

            // client->ping();
        }
        else if (type == WS_EVT_DATA)
        {
            handleWebSocketMessage(client, arg, data, len);
        }
        else if (type == WS_EVT_DISCONNECT)
        {
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
        }
        else if (type == WS_EVT_ERROR)
        {
            Serial.printf("WebSocket client #%u error\n", client->id());
        }
        else if (type == WS_EVT_PONG)
        {
            Serial.printf("WebSocket client #%u pong received\n", client->id());
        }
    }

    // Messages bigger than one TCP packet (settings import, large functions) arrive in several chunks,
    // they are collected here and processed once complete
    static constexpr size_t MAX_WS_MESSAGE_SIZE = 16384;
    static std::vector<uint8_t> pendingMessage;
    static uint32_t pendingClientId = 0;

    void handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len)
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->final && info->num == 0 && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
        {
            processWebSocketMessage(client, data, len);
            return;
        }

        if (info->message_opcode != WS_TEXT)
        {
            return;
        }

        // First chunk of the first frame starts a new message
        if (info->num == 0 && info->index == 0)
        {
            pendingMessage.clear();
            pendingClientId = client->id();
        }

        // Chunks of another client's message can't be mixed in, that message is dropped
        if (client->id() != pendingClientId || pendingMessage.size() + len > MAX_WS_MESSAGE_SIZE)
        {
            pendingMessage.clear();
            pendingMessage.shrink_to_fit();
            pendingClientId = 0;
            return;
        }

        if (pendingMessage.empty() && info->num == 0 && info->len <= MAX_WS_MESSAGE_SIZE)
        {
            pendingMessage.reserve(info->len);
        }
        pendingMessage.insert(pendingMessage.end(), data, data + len);

        if (info->final && info->index + len == info->len)
        {
            processWebSocketMessage(client, pendingMessage.data(), pendingMessage.size());
            // Free the buffer, imports are rare and the memory is better left to the heap
            pendingMessage.clear();
            pendingMessage.shrink_to_fit();
            pendingClientId = 0;
        }
    }

    void processWebSocketMessage(AsyncWebSocketClient *client, const uint8_t *data, size_t len)
    {
        JsonDocument received;
        deserializeJson(received, data, len);

        String messageType = received["messageType"].as<String>();
        JsonObject payload = received["payload"].as<JsonObject>();

        JsonDocument response;
        JsonObject responsePayload = response["payload"].to<JsonObject>();
        JsonObject responseStatus = response["status"].to<JsonObject>();
        response["messageType"] = messageType + "Response";

        switch (su::SH(messageType.c_str()))
        {
            // case su::SH("ping"):
            //     response["payload"] = "pong";
            //     break;

        case su::SH("setWifiSettings"):
            setWifiSettings(payload, responseStatus);
            break;

        case su::SH("getWifiSettings"):
            getWifiSettings(responsePayload);
            break;

        case su::SH("startWifiScan"):
            startWifiScan();
            break;

        case su::SH("startDeviceScan"):
            if (!MyHeatDiscovery::startScan())
            {
                setWarningMessage(responseStatus, "Пошук вже виконується");
            }
            break;

        case su::SH("getDiscoveredTemperatureSensors"):
            getDiscoveredTemperatureSensors(responsePayload, responseStatus);
            break;

        case su::SH("setTemperatureSensor"):
            setTemperatureSensor(payload, responseStatus);
            break;

        case su::SH("deleteTemperatureSensor"):
            deleteTemperatureSensor(payload, responseStatus);
            break;

        case su::SH("getTemperatureSensorsSettings"):
            getTemperatureSensorsSettings(responsePayload);
            break;

        case su::SH("setTemperatureSensorsSettings"):
            setTemperatureSensorsSettings(payload, responseStatus);
            break;

        case su::SH("getRelaysSettings"):
            getRelaysSettings(responsePayload);
            break;

        case su::SH("setRelayMode"):
            setRelayMode(payload, responseStatus);
            break;

        case su::SH("setRelaysSettings"):
            setRelaysSettings(payload, responseStatus);
            break;

        case su::SH("setRelayCount"):
            setRelayCount(payload, responseStatus);
            break;

        case su::SH("getRelayCount"):
            getRelayCount(responsePayload);
            break;

        case su::SH("setFunctionIsEnabled"):
            setFunctionIsEnabled(payload, responseStatus);
            break;

        case su::SH("setFunctionsSettings"):
            setFunctionsSettings(payload, responseStatus);
            break;

        case su::SH("getPinsData"):
            getUsedPinsData(responsePayload);
            break;

        case su::SH("getNTPSettings"):
            getNTPSettings(responsePayload);
            break;

        case su::SH("setNTPSettings"):
            setNTPSettings(payload, responseStatus);
            break;

        case su::SH("getTelegramBotSettings"):
            getTelegramBotSettings(responsePayload);
            break;

        case su::SH("setTelegramBotSettings"):
            setTelegramBotSettings(payload, responseStatus);
            break;

        case su::SH("deleteTelegramUser"):
            deleteTelegramUser(payload, responseStatus);
            break;

        case su::SH("getHardwareIOSettings"):
            getHardwareIOSettings(responsePayload);
            break;

        case su::SH("setHardwareIOSettings"):
            setHardwareIOSettings(payload, responseStatus);
            break;

        case su::SH("getAllDeviceSettings"):
            getAllDeviceSettings(responsePayload);
            break;

        case su::SH("setAllDeviceSettings"):
            setAllDeviceSettings(payload, responseStatus);
            break;

        case su::SH("getMqttSettings"):
            getMqttSettings(responsePayload);
            break;

        case su::SH("setMqttSettings"):
            setMqttSettings(payload, responseStatus);
            break;

        case su::SH("restartDevice"):
            setWarningMessage(responseStatus, "Перезавантаження пристрою");
            restartRequired = true;
            break;

        case su::SH("getSmokeSensorSettings"):
            getSmokeSensorSettings(responsePayload);
            break;

        case su::SH("setSmokeSensorSettings"):
            setSmokeSensorSettings(payload, responseStatus);
            break;

        case su::SH("getSmokeSensor"):
            getSmokeSensor(responsePayload);
            break;

        default:
            setErrorMessage(responseStatus, "Невідовий тип повідомлення");
            break;
        }

        String responseString;
        serializeJson(response, responseString);

        client->text(responseString);

        if (messageType.startsWith("set") || messageType.startsWith("del"))
        {
            MyHeatDevice::getInstance().manualTick();
            requestFullSend();

            // The device name can change with these, the controller list and tab title follow it
            if (messageType == "setWifiSettings" || messageType == "setAllDeviceSettings")
            {
                sendDeviceInfo();
            }
        }
    }

    // The web UI marks the connection inactive after 8s without messages. Data is only sent when it changes,
    // so a tiny heartbeat message fills the silence instead of resending everything
    static constexpr uint32_t HEARTBEAT_INTERVAL = 4000;
    static constexpr uint32_t HOUSEKEEPING_INTERVAL = 2000;

    static MyHeatUtils::ChangeTracker messageTracker;
    static uint32_t lastHeartbeatTick = 0;
    static uint32_t lastStateRevision = 0;
    // Set from the async websocket task, the send itself happens in tick() on the main loop
    static volatile bool isFullSendRequested = false;

    void sendDataToClients(JsonDocument &data, const String &messageType)
    {
        data[F("messageType")] = messageType;
        String jsonString;
        serializeJson(data, jsonString);

        websocket.textAll(jsonString);
        lastHeartbeatTick = millis();
    }

    void sendHeartbeat()
    {
        websocket.textAll(F("{\"messageType\":\"heartbeat\"}"));
        lastHeartbeatTick = millis();
    }

    void sendIfChanged(JsonDocument &data, RepeatableMessage slot, const String &messageType, bool force)
    {
        // The hash is streamed from the document, the String is only built when the message is really sent
        bool isChanged = messageTracker.update(slot, MyHeatUtils::hashJson(data));
        if (isChanged || force)
        {
            sendDataToClients(data, messageType);
        }
    }

    void sendTemperaturesData(bool force)
    {
        JsonDocument response;
        response["payload"] = getTemperatureSensorsData();
        sendIfChanged(response, MSG_TEMPERATURES, F("temperaturesData"), force);
    }

    void sendRelaysData(bool force)
    {
        JsonDocument response;
        response["payload"] = getRelaysData();
        sendIfChanged(response, MSG_RELAYS, F("relaysData"), force);
    }

    void sendUsedPinsData()
    {
        JsonDocument response;
        getUsedPinsData(response["payload"].to<JsonObject>());
        sendDataToClients(response, F("getPinsDataResponse"));
    }

    void sendDeviceInfo()
    {
        JsonDocument response;
        getDeviceInfo(response["payload"].to<JsonObject>());
        sendDataToClients(response, F("deviceInfo"));
    }

    void sendDeviceScanData()
    {
        std::vector<MyHeatDiscovery::Device> devices;
        if (!MyHeatDiscovery::takeResults(devices))
            return;

        JsonDocument response;
        JsonArray payload = response["payload"].to<JsonArray>();
        for (const auto &device : devices)
        {
            JsonObject item = payload.add<JsonObject>();
            item["name"] = device.name;
            item["mdns"] = device.hostname;
            item["address"] = device.address;
        }

        JsonObject status = response["status"].to<JsonObject>();
        if (devices.empty())
        {
            setWarningMessage(status, F("Інших контролерів не знайдено"));
        }
        else
        {
            setSuccessMessage(status, "Знайдено контролерів: " + String(devices.size()));
        }

        sendDataToClients(response, F("deviceScanData"));
    }

    void sendFunctionsData(bool force)
    {
        JsonDocument response;
        response["payload"] = getFunctionsData();
        sendIfChanged(response, MSG_FUNCTIONS, F("functionsData"), force);
    }

    void sendNetworksData()
    {
        if (isScanCompleted())
        {
            JsonDocument response;
            response["payload"] = getNetworksData();

            if (response["payload"].size() == 0)
            {
                JsonObject status = response["status"].to<JsonObject>();
                setWarningMessage(status, F("Мережі не знайдено"));
            }
            else
            {
                setSuccessMessage(response["status"].to<JsonObject>(), F("Сканування завершено"));
            }

            sendDataToClients(response, F("wifiScanData"));
        }
    }

    bool canSendData()
    {
        MyHeatWifi &wifi = MyHeatWifi::getInstance();
        return websocket.count() > 0 && (wifi.isConnected() || wifi.hasAPClients());
    }

    void sendAlertNotification(const String& message)
    {
        if (canSendData())
        {
            JsonDocument response;
            JsonObject responseStatus = response["status"].to<JsonObject>();
            response["messageType"] = F("alertNotification");

            setErrorMessage(responseStatus, message);

            sendDataToClients(response, F("alertNotification"));
        }
    }

    void sendRepeatableDataToClients(bool force)
    {
        if (canSendData())
        {
            sendTemperaturesData(force);
            sendRelaysData(force);
            sendFunctionsData(force);
            MyHeatWifi::getInstance().setIsWebsocketClientsConnected(true);
        }
        else
        {
            MyHeatWifi::getInstance().setIsWebsocketClientsConnected(false);
        }
    }

    void requestFullSend()
    {
        isFullSendRequested = true;
    }

    void tick()
    {
        bool isFullSend = isFullSendRequested;
        uint32_t stateRevision = MyHeatDevice::getInstance().getStateRevision();

        // Nothing is serialized until the device refreshed its state or a full send is due
        if (isFullSend || stateRevision != lastStateRevision)
        {
            isFullSendRequested = false;
            lastStateRevision = stateRevision;
            sendRepeatableDataToClients(isFullSend);
        }

        if (millis() - lastHeartbeatTick > HEARTBEAT_INTERVAL && canSendData())
        {
            sendHeartbeat();
        }

        if (millis() - lastSendTick > HOUSEKEEPING_INTERVAL)
        {
            lastSendTick = millis();

            if (canSendData())
            {
                sendNetworksData();
                sendDeviceScanData();
            }

            websocket.cleanupClients();

            if (restartRequired)
            {
                ESP.restart();
            }
        }
    }
}