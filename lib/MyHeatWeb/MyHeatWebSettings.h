#ifndef MYHEATWEBSETTINGS_H
#define MYHEATWEBSETTINGS_H

#include <ArduinoJson.h>
#include "MyHeatDevice.h"
#include "MyHeatWifi.h"

namespace MyHeatWeb
{
    void getUsedPinsData(JsonObject response);
    void setWifiSettings(JsonObject payload);
    void getWifiSettings(JsonObject response);
    void startWifiScan();
    bool isScanCompleted();
    JsonDocument getNetworksData();
}

#endif