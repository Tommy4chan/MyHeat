#pragma once

#include <StringUtils.h>
#include "MyHeatUtils.h"
#include "Utils.h"
#include "MyHeatCustomFunction.h"
#include "MyHeatRelay.h"

namespace MyHeatTelebot
{
    String getRelayScreenText(MyHeatRelay *relaysPtr);
    String getTemperatureScreenText();
    String getFunctionsListScreenText(MyHeatCustomFunction *customFunctionsPtr);
    String getFunctionScreenText(MyHeatCustomFunction customFunction, byte functionIndex);
}