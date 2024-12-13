#include "MyHeatWeb.h"

namespace MyHeatWeb
{
    void begin()
    {
        server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

        AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/api/wifi/set", [](AsyncWebServerRequest *request, JsonVariant &json)
                                                                               {
            const JsonObject& jsonObj = json.as<JsonObject>();
            myHeatWifi.setWifiCredentials(jsonObj["ssid"], jsonObj["password"]);
            request->send(200, "text/plain", "Wifi settings updated"); });

        server.addHandler(handler);

        server.on("/api/wifi/get", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "application/json", "{\"ssid\":\"" + myHeatWifi.getSSID() + "\",\"password\":\"" + myHeatWifi.getPassword() + "\"}"); });

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

            if (messageType == "setWifi")
            {
                myHeatWifi.setWifiCredentials(payload["ssid"], payload["password"]);
            }
            else if (messageType == "getWifiSettings")
            {
                response["payload"]["ssid"] = myHeatWifi.getSSID();
                response["payload"]["password"] = myHeatWifi.getPassword();
            }
            else if (messageType == "startWifiScan")
            {
                myHeatWifi.startWifiScan();
            }
            else
            {
                response["error"] = "Unknown message type";
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
        JsonDocument temperaturesData;
        copyArray(myHeatDevice.getTemperatures(), myHeatDevice.getTemperatureCount(), temperaturesData[F("temperatures")]);

        sendDataToClients(temperaturesData, F("temperaturesData"));
    }

    void sendRelaysData()
    {
        JsonDocument relaysData;

        for (int i = 0; i < RELAY_COUNT; i++)
        {
            MyHeatRelay relay = myHeatDevice.getRelay(i);
            relaysData[F("relays")][i][F("mode")] = relay.getMode();
            relaysData[F("relays")][i][F("isActive")] = relay.getIsActive();
        }

        sendDataToClients(relaysData, F("relaysData"));
    }

    void sendFunctionsData()
    {
        JsonDocument functionsData;

        for (int i = 0; i < FUNCTION_COUNT; i++)
        {
            MyHeatCustomFunction function = myHeatDevice.getCustomFunction(i);

            functionsData[F("functions")][i][F("sign")] = function.getSign();
            functionsData[F("functions")][i][F("temperatureIndex")][0] = function.getTemperatureIndex(0);
            functionsData[F("functions")][i][F("temperatureIndex")][1] = function.getTemperatureIndex(1);
            functionsData[F("functions")][i][F("deltaValue")][0] = function.getDeltaValue(0);
            functionsData[F("functions")][i][F("deltaValue")][1] = function.getDeltaValue(1);
            functionsData[F("functions")][i][F("relayIndex")] = function.getRelayIndex();
            functionsData[F("functions")][i][F("isEnabled")] = function.getIsEnabled();
            functionsData[F("functions")][i][F("isActive")] = function.getIsActive();
        }

        sendDataToClients(functionsData, F("functionsData"));
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