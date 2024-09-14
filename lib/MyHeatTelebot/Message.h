#pragma once

#include <StringUtils.h>
#include "MyHeatUtils.h"
#include "Utils.h"
#include "CustomFunction.h"

namespace MyHeatTelebot
{
    String getRelayScreenText();
    String getTemperatureScreenText();
    String getFunctionsListScreenText(CustomFunction *customFunctionsPtr);
    String getFunctionScreenText(CustomFunction customFunction, byte functionIndex);
}