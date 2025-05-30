#include "MyHeatWeb.h"

namespace MyHeatWeb
{
    void begin()
    {
        server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html").setCacheControl("max-age=6000000");
        websocket.enable(true);
        setupWebsocket();
        server.begin();
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
            sendRepeatableDataToClients();

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
                sendRepeatableDataToClients();
            }
        }
    }

    void sendDataToClients(JsonDocument data, String messageType)
    {
        JsonDocument response = data;
        response[F("messageType")] = messageType;
        String jsonString;
        serializeJson(response, jsonString);

        websocket.textAll(jsonString);
    }

    void sendTemperaturesData()
    {
        JsonDocument response;
        response["payload"] = getTemperatureSensorsData();
        sendDataToClients(response, F("temperaturesData"));
    }

    void sendRelaysData()
    {
        JsonDocument response;
        response["payload"] = getRelaysData();
        sendDataToClients(response, F("relaysData"));
    }

    void sendUsedPinsData()
    {
        JsonDocument response;
        getUsedPinsData(response["payload"].to<JsonObject>());
        sendDataToClients(response, F("getPinsDataResponse"));
    }

    void sendFunctionsData()
    {
        JsonDocument response;
        response["payload"] = getFunctionsData();
        sendDataToClients(response, F("functionsData"));
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
        return websocket.count() > 0 && (WiFi.isConnected() || WiFi.softAPgetStationNum() > 0);
    }

    void sendAlertNotification(String message)
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

    void sendRepeatableDataToClients()
    {
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

        if (canSendData())
        {
            sendTemperaturesData();
            sendRelaysData();
            sendFunctionsData();
            sendNetworksData();
            MyHeatWifi::getInstance().setIsWebsocketClientsConnected(true);
        }
        else {
            MyHeatWifi::getInstance().setIsWebsocketClientsConnected(false);
        }

        lastSendTick = millis();
    }

    void tick()
    {
        if (millis() - lastSendTick > 2000)
        {
            sendRepeatableDataToClients();

            websocket.cleanupClients();

            if (restartRequired)
            {
                ESP.restart();
            }
        }
    }
}