#ifndef MYHEATWEB_H
#define MYHEATWEB_H

#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <MyHeatDevice.h>
#include <MyHeatWifi.h>
#include "ArduinoJson.h"
#include "AsyncJson.h"
#include <StringUtils.h>
#include "MyHeatWebTemperatures.h"
#include "MyHeatWebRelays.h"
#include "MyHeatWebCustomFunctions.h"
#include "MyHeatWebSettings.h"
#include "MyHeatWebUtils.h"

namespace MyHeatWeb
{
    inline AsyncWebServer server(80);
    inline AsyncWebSocket websocket("/ws");

    inline uint32_t lastSendTick = 0;
    inline bool restartRequired = false;

    void begin();

    void setupWebsocket();
    void wsEventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len);

    void sendTemperaturesData();
    void sendRelaysData();
    void sendFunctionsData();
    void sendUsedPinsData();
    void sendNetworksData();
    void sendDataToClients(JsonDocument data, String messageType);

    bool canSendData();
    void sendAlertNotification(String message);
    void sendRepeatableDataToClients();
    void tick();
}

#endif
