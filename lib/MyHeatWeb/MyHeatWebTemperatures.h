#ifndef MYHEATWEBTEMPERATURES_H
#define MYHEATWEBTEMPERATURES_H

#include <ArduinoJson.h>
#include <MyHeatDevice.h>
#include "MyHeatHardwareIO.h"

namespace MyHeatWeb
{
    void getDiscoveredTemperatureSensors(JsonDocument &response);
    void setTemperatureSensor(JsonObject payload);
    void deleteTemperatureSensor(JsonObject payload);
    void getTemperatureSensorsSettings(JsonDocument &response);
    void setTemperatureSensorsSettings(JsonObject payload);
    JsonDocument getTemperatureSensorsData();
}

#endif