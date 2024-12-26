#ifndef MYHEATWEBSETTINGS_H
#define MYHEATWEBSETTINGS_H

#include <ArduinoJson.h>
#include "MyHeatDevice.h"
#include "MyHeatWifi.h"

namespace MyHeatWeb
{
    JsonDocument getUsedPinsData();
    void setWifiSettings(JsonObject payload);
    void getWifiSettings(JsonDocument &response);
    void startWifiScan();
    bool isScanCompleted();
    JsonDocument getNetworksData();
}

#endif