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

    // Repeatable messages are sent when their content changes, plus a periodic full send as a heartbeat
    enum RepeatableMessage : byte { MSG_TEMPERATURES = 0, MSG_RELAYS, MSG_FUNCTIONS, MSG_COUNT };

    void begin();
    void setupOTA();

    void setupWebsocket();
    void wsEventHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
    void handleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len);
    void processWebSocketMessage(AsyncWebSocketClient *client, const uint8_t *data, size_t len);

    void sendTemperaturesData(bool force = true);
    void sendRelaysData(bool force = true);
    void sendFunctionsData(bool force = true);
    void sendUsedPinsData();
    void sendNetworksData();
    void sendDeviceInfo();
    void sendHeartbeat();
    void sendDeviceScanData();
    void sendDataToClients(JsonDocument &data, const String &messageType);
    void sendIfChanged(JsonDocument &data, RepeatableMessage slot, const String &messageType, bool force);

    bool canSendData();
    void sendAlertNotification(const String& message);
    void sendRepeatableDataToClients(bool force);
    void requestFullSend();
    void tick();
}

#endif
