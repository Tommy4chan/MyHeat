#ifndef MYHEATWEBSETTINGS_H
#define MYHEATWEBSETTINGS_H

#include <ArduinoJson.h>
#include "MyHeatDevice.h"
#include "MyHeatWifi.h"
#include "MyHeatTelebot.h"
#include "MyHeatHardwareIO.h"
#include "MyHeatWebUtils.h"

namespace MyHeatWeb
{
    void getUsedPinsData(JsonObject response);
    void setWifiSettings(JsonObject payload, JsonObject status);
    void getWifiSettings(JsonObject response);
    void setNTPSettings(JsonObject payload, JsonObject status);
    void getNTPSettings(JsonObject response);
    void setTelegramBotSettings(JsonObject payload, JsonObject status);
    void getTelegramBotSettings(JsonObject response);
    void setHardwareIOSettings(JsonObject payload, JsonObject status);
    void getHardwareIOSettings(JsonObject response);
    void getAllDeviceSettings(JsonObject response);
    void setAllDeviceSettings(JsonObject payload, JsonObject status);
    void startWifiScan();
    bool isScanCompleted();
    JsonDocument getNetworksData();
}

#endif