#ifndef MYHEATWEBRELAYS_H
#define MYHEATWEBRELAYS_H

#include <ArduinoJson.h>
#include <MyHeatDevice.h>
#include "MyHeatHardwareIO.h"

namespace MyHeatWeb
{
    void getRelaysSettings(JsonObject response);
    void setRelayMode(JsonObject payload);
    void setRelaysSettings(JsonObject payload);
    void setRelayCount(JsonObject payload);
    void getRelayCount(JsonObject response);
    JsonDocument getRelaysData();
}

#endif