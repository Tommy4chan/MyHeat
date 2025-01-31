#include "MyHeatWeb.h"

namespace MyHeatWeb
{
    void begin()
    {
        server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");
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

            client->ping();
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
            JsonObject responseError = response["error"].to<JsonObject>();
            response["messageType"] = messageType + "Response";

            switch (su::SH(messageType.c_str()))
            {
                // case su::SH("ping"):
                //     response["payload"] = "pong";
                //     break;

            case su::SH("setWifiSettings"):
                setWifiSettings(payload);
                break;

            case su::SH("getWifiSettings"):
                getWifiSettings(responsePayload);
                break;

            case su::SH("startWifiScan"):
                startWifiScan();
                break;

            case su::SH("getDiscoveredTemperatureSensors"):
                getDiscoveredTemperatureSensors(responsePayload);
                break;

            case su::SH("setTemperatureSensor"):
                setTemperatureSensor(payload, responseError);
                break;

            case su::SH("deleteTemperatureSensor"):
                deleteTemperatureSensor(payload);
                break;

            case su::SH("getTemperatureSensorsSettings"):
                getTemperatureSensorsSettings(responsePayload);
                break;

            case su::SH("setTemperatureSensorsSettings"):
                setTemperatureSensorsSettings(payload);
                break;

            case su::SH("getRelaysSettings"):
                getRelaysSettings(responsePayload);
                break;

            case su::SH("setRelayMode"):
                setRelayMode(payload);
                break;

            case su::SH("setRelaysSettings"):
                setRelaysSettings(payload);
                break;

            case su::SH("setRelayCount"):
                setRelayCount(payload);
                break;

            case su::SH("getRelayCount"):
                getRelayCount(responsePayload);
                break;

            case su::SH("setFunctionIsEnabled"):
                setFunctionIsEnabled(payload);
                break;

            case su::SH("setFunctionsSettings"):
                setFunctionsSettings(payload);
                break;

            case su::SH("getPinsData"):
                getUsedPinsData(responsePayload);
                break;

            default:
                response["error"] = "Unknown message type";
                break;
            }

            String responseString;
            serializeJson(response, responseString);

            client->text(responseString);

            if (messageType.startsWith("set") || messageType.startsWith("del"))
            {
                sendRepeatableDataToClients();
            }
        }
    }

    void sendDataToClients(JsonDocument data, String messageType)
    {
        JsonDocument response;
        response[F("payload")] = data;
        response[F("messageType")] = messageType;
        String jsonString;
        serializeJson(response, jsonString);

        websocket.textAll(jsonString);
    }

    void sendTemperaturesData()
    {
        sendDataToClients(getTemperatureSensorsData(), F("temperaturesData"));
    }

    void sendRelaysData()
    {
        sendDataToClients(getRelaysData(), F("relaysData"));
    }

    void sendUsedPinsData()
    {
        JsonDocument response;
        getUsedPinsData(response.to<JsonObject>());
        sendDataToClients(response, F("getPinsDataResponse"));
    }

    void sendFunctionsData()
    {
        sendDataToClients(getFunctionsData(), F("functionsData"));
    }

    void sendNetworksData()
    {
        if (isScanCompleted())
        {
            sendDataToClients(getNetworksData(), F("wifiScanData"));
        }
    }

    void sendRepeatableDataToClients()
    {
        sendTemperaturesData();
        sendRelaysData();
        sendFunctionsData();
        sendNetworksData();

        lastSendTick = millis();
    }

    void tick()
    {
        if (websocket.count() > 0 && millis() - lastSendTick > 1000)
        {
            sendRepeatableDataToClients();

            websocket.cleanupClients();
        }
    }
}