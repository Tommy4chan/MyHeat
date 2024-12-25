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
            client->ping();

            sendUsedPinsData();
            sendTemperaturesData();
            sendRelaysData();
            sendFunctionsData();
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
            response["messageType"] = messageType + "Response";

            switch (su::SH(messageType.c_str()))
            {
            case su::SH("setWifi"):
                myHeatWifi.setWifiCredentials(payload["ssid"], payload["password"]);
                break;

            case su::SH("getWifiSettings"):
                response["payload"]["ssid"] = myHeatWifi.getSSID();
                response["payload"]["password"] = myHeatWifi.getPassword();
                break;

            case su::SH("startWifiScan"):
                myHeatWifi.startWifiScan();
                break;

            case su::SH("getDiscoveredTemperatureSensors"):
                getDiscoveredTemperatureSensors(response);
                break;

            case su::SH("setTemperatureSensor"):
                setTemperatureSensor(payload);
                break;

            case su::SH("deleteTemperatureSensor"):
                deleteTemperatureSensor(payload);
                break;

            case su::SH("getTemperatureSensorsSettings"):
                getTemperatureSensorsSettings(response);
                break;

            case su::SH("setTemperatureSensorsSettings"):
                setTemperatureSensorsSettings(payload);
                break;

            case su::SH("getRelaysSettings"):
                getRelaysSettings(response);
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
                getRelayCount(response);
                break;

            case su::SH("setFunctionIsEnabled"):
                setFunctionIsEnabled(payload);
                break;

            case su::SH("setFunctionsSettings"):
                setFunctionsSettings(payload);
                break;

            default:
                response["error"] = "Unknown message type";
                break;
            }

            String responseString;
            serializeJson(response, responseString);

            client->text(responseString);
        }
    }

    void sendDataToClients(JsonDocument data, String messageType)
    {
        data[F("messageType")] = messageType;
        String jsonString;
        serializeJson(data, jsonString);

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
        sendDataToClients(getUsedPinsData(), F("usedPinsData"));
    }

    void sendFunctionsData()
    {
        sendDataToClients(getFunctionsData(), F("functionsData"));
    }

    void tick()
    {
        static unsigned long lastSend = 0;
        unsigned long now = millis();

        if (websocket.count() > 0 && now - lastSend > 1000)
        {
            if (myHeatWifi.isScanCompleted() >= 0)
            {
                sendDataToClients(myHeatWifi.getNetworks(), F("wifiScanData"));
            }
            sendTemperaturesData();
            sendRelaysData();
            sendFunctionsData();
            lastSend = now;
        }

        websocket.cleanupClients();
    }
}