#include "MyHeatWeb.h"
#include <MyHeatAlerts.h>
#include <Update.h>

namespace MyHeatWeb
{
    void begin()
    {
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

        server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setCacheControl("max-age=6000000");
        setupOTA();
        websocket.enable(true);
        setupWebsocket();
        server.begin();
        MyHeatAlerts::registerHandler(sendAlertNotification);
    }

    void setupOTA()
    {
        server.on("/update", HTTP_OPTIONS, [](AsyncWebServerRequest *request) {
            request->send(200);
        });

        server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request) {
            bool shouldReboot = !Update.hasError();
            AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", shouldReboot ? "OK" : "FAIL");
            response->addHeader("Connection", "close");
            request->send(response);
            
            if (shouldReboot) {
                MyHeatWeb::restartRequired = true;
            }
        }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
            if (!index) {
                Serial.printf("Update Start: %s\n", filename.c_str());
                int cmd = U_FLASH;
                if (request->hasParam("type") && request->getParam("type")->value() == "fs") {
                    cmd = U_SPIFFS;
                }
                
                if (!Update.begin(request->contentLength(), cmd)) {
                    Update.printError(Serial);
                }
            }
            if (!Update.hasError()) {
                if (Update.write(data, len) != len) {
                    Update.printError(Serial);
                }
            }
            if (final) {
                if (Update.end(true)) {
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

    void handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len)
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
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
            }
        }
    }

    // The web UI marks the connection inactive after 8s without messages
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

        if (force)
        {
            lastHeartbeatTick = millis();
        }
    }

    void requestFullSend()
    {
        isFullSendRequested = true;
    }

    void tick()
    {
        bool isFullSend = isFullSendRequested || millis() - lastHeartbeatTick > HEARTBEAT_INTERVAL;
        uint32_t stateRevision = MyHeatDevice::getInstance().getStateRevision();

        // Nothing is serialized until the device refreshed its state or a full send is due
        if (isFullSend || stateRevision != lastStateRevision)
        {
            isFullSendRequested = false;
            lastStateRevision = stateRevision;
            sendRepeatableDataToClients(isFullSend);
        }

        if (millis() - lastSendTick > HOUSEKEEPING_INTERVAL)
        {
            lastSendTick = millis();

            if (canSendData())
            {
                sendNetworksData();
            }

            websocket.cleanupClients();

            if (restartRequired)
            {
                ESP.restart();
            }
        }
    }
}