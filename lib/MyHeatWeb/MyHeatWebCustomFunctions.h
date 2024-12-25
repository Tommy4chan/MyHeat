#ifndef MYHEATWEBCUSTOMFUNCTIONS_H
#define MYHEATWEBCUSTOMFUNCTIONS_H

#include <ArduinoJson.h>
#include <MyHeatDevice.h>

namespace MyHeatWeb
{
    void setFunctionIsEnabled(JsonObject payload);
    void setFunctionsSettings(JsonObject payload);
    JsonDocument getFunctionsData();
}

#endif