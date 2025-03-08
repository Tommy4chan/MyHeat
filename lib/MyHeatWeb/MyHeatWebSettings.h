#ifndef MYHEATWEBSETTINGS_H
#define MYHEATWEBSETTINGS_H

#include <ArduinoJson.h>
#include "MyHeatDevice.h"
#include "MyHeatWifi.h"
#include "MyHeatTelebot.h"
#include "MyHeatHardwareIO.h"

namespace MyHeatWeb
{
    void getUsedPinsData(JsonObject response);
    void setWifiSettings(JsonObject payload);
    void getWifiSettings(JsonObject response);
    void setNTPSettings(JsonObject payload);
    void getNTPSettings(JsonObject response);
    void setTelegramBotSettings(JsonObject payload);
    void getTelegramBotSettings(JsonObject response);
    void setHardwareIOSettings(JsonObject payload);
    void getHardwareIOSettings(JsonObject response);
    void getAllDeviceSettings(JsonObject response);
    void setAllDeviceSettings(JsonObject payload);
    void startWifiScan();
    bool isScanCompleted();
    JsonDocument getNetworksData();
}

#endif