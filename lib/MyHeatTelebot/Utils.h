#pragma once

#include <Arduino.h>
#include <StringUtils.h>
#include "MyHeatUtils.h"

namespace MyHeatTelebot
{
    String getConvertedFunctionToText(byte functionIndex);
    Text getCallbackFromQuery(Text query);
    byte getValueFromQuery(String query);
}