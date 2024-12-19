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
            else if (messageType == "getDiscoveredTemperatureSensors")
            {
                byte count = myHeatDevice.discoverTemperatureSensor();
                uint8_t **addresses = myHeatDevice.getDiscoveredTemperatureSensorAddresses();

                for (int i = 0; i < count; i++)
                {
                    response["payload"]["discoveredTemperatureSensors"][i] = MyHeatUtils::getAddressToString(addresses[i]);
                }
            }
            else if (messageType == "setTemperatureSensor")
            {
                myHeatDevice.setTemperatureSensorAddress(payload["tempIndex"], payload["sensorAddressIndex"]);
            }
            else if (messageType == "deleteTemperatureSensor")
            {
                myHeatDevice.deleteTemperatureSensorAddress(payload["tempIndex"]);
            }
            else if (messageType == "getTemperatureSensorsSettings")
            {
                response["payload"]["temperaturePin"] = myHeatDevice.getTemperaturePin();
                response["payload"]["temperatureCount"] = myHeatDevice.getTemperatureCount();
            }
            else if (messageType == "setTemperatureSensorsSettings")
            {
                myHeatDevice.updateTemperatureSensorsSettings(payload["temperaturePin"], payload["temperatureCount"]);
            }
            else if (messageType == "getRelaysSettings")
            {
                for (int i = 0; i < myHeatDevice.getRelayCount(); i++)
                {
                    response["payload"]["relays"][i]["pin"] = myHeatDevice.getRelay(i).getPin();
                    response["payload"]["relays"][i]["isActiveOnHigh"] = myHeatDevice.getRelay(i).getIsActiveOnHigh();
                }
            }
            else if (messageType == "setRelayMode")
            {
                myHeatDevice.setRelayMode(payload["relayIndex"], payload["relayMode"]);
            }
            else if (messageType == "setRelaysSettings")
            {
                myHeatDevice.updateRelaysSettings(payload);
            }
            else if (messageType == "setRelayCount")
            {
                myHeatDevice.updateRelayCount(payload["relayCount"]);
            }
            else if (messageType == "getRelayCount")
            {
                response["payload"]["relayCount"] = myHeatDevice.getRelayCount();
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

        for (int i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            MyHeatRelay relay = myHeatDevice.getRelay(i);
            relaysData[F("relays")][i][F("mode")] = relay.getMode();
            relaysData[F("relays")][i][F("isActive")] = relay.getIsActive();
        }

        sendDataToClients(relaysData, F("relaysData"));
    }

    void sendUsedPinsData()
    {
        JsonDocument usedPinsData;

        usedPinsData[F("usedPins")][0] = myHeatDevice.getTemperaturePin();
        usedPinsData[F("usedPins")][1] = ENC_A;
        usedPinsData[F("usedPins")][2] = ENC_B;
        usedPinsData[F("usedPins")][3] = ENC_BTN;
        usedPinsData[F("usedPins")][4] = OLED_SCL;
        usedPinsData[F("usedPins")][5] = OLED_SDA;

        for (int i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            usedPinsData[F("usedPins")][i + 6] = myHeatDevice.getRelay(i).getPin();
        }

        sendDataToClients(usedPinsData, F("usedPinsData"));
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