#ifndef MYHEATWEBTEMPERATURES_H
#define MYHEATWEBTEMPERATURES_H

#include <ArduinoJson.h>
#include <MyHeatDevice.h>
#include "MyHeatHardwareIO.h"
#include "MyHeatWebUtils.h"

namespace MyHeatWeb
{
    void getDiscoveredTemperatureSensors(JsonObject response);
    void setTemperatureSensor(JsonObject payload, JsonObject status);
    void deleteTemperatureSensor(JsonObject payload);
    void getTemperatureSensorsSettings(JsonObject response);
    void setTemperatureSensorsSettings(JsonObject payload);
    JsonDocument getTemperatureSensorsData();
}

#endif