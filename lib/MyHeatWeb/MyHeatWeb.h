#ifndef MYHEATWEB_H
#define MYHEATWEB_H

#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <MyHeatDevice.h>
#include <MyHeatWifi.h>
#include "ArduinoJson.h"
#include "AsyncJson.h"

namespace MyHeatWeb
{
    inline AsyncWebServer server(80);
    inline AsyncWebSocket websocket("/ws");

    inline MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
    inline MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

    void begin();

    void setupWebsocket();
    void wsEventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len);

    void sendTemperaturesData();
    void sendRelaysData();
    void sendFunctionsData();
    void sendDataToClients(JsonDocument data, String messageType);

    void tick();
}
#endif
