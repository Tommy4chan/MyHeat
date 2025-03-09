#ifndef MYHEATWEBTEMPERATURES_H
#define MYHEATWEBTEMPERATURES_H

#include <ArduinoJson.h>
#include <MyHeatDevice.h>
#include "MyHeatHardwareIO.h"
#include "MyHeatWebUtils.h"

namespace MyHeatWeb
{
    void getDiscoveredTemperatureSensors(JsonObject response, JsonObject status);
    void setTemperatureSensor(JsonObject payload, JsonObject status);
    void deleteTemperatureSensor(JsonObject payload, JsonObject status);
    void getTemperatureSensorsSettings(JsonObject response);
    void setTemperatureSensorsSettings(JsonObject payload, JsonObject status);
    JsonDocument getTemperatureSensorsData();
}

#endif