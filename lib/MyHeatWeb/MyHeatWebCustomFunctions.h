#ifndef MYHEATWEBCUSTOMFUNCTIONS_H
#define MYHEATWEBCUSTOMFUNCTIONS_H

#include <ArduinoJson.h>
#include <MyHeatDevice.h>
#include "MyHeatWebUtils.h"

namespace MyHeatWeb
{
    void setFunctionIsEnabled(JsonObject payload, JsonObject status);
    void setFunctionsSettings(JsonObject payload, JsonObject status);
    JsonDocument getFunctionsData();
}

#endif