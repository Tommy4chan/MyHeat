#ifndef MYHEATWEBRELAYS_H
#define MYHEATWEBRELAYS_H

#include <ArduinoJson.h>
#include <MyHeatDevice.h>

namespace MyHeatWeb
{
    void getRelaysSettings(JsonDocument &response);
    void setRelayMode(JsonObject payload);
    void setRelaysSettings(JsonObject payload);
    void setRelayCount(JsonObject payload);
    void getRelayCount(JsonDocument &response);
    JsonDocument getRelaysData();
}

#endif