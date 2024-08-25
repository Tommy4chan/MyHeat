#pragma once

#include <StringUtils.h>
#include "MyHeatUtils.h"
#include "Utils.h"

namespace MyHeatTelebot
{
    String getRelayScreenText();
    String getTemperatureScreenText();
    String getFunctionsListScreenText();
    String getFunctionScreenText(byte functionIndex);
}