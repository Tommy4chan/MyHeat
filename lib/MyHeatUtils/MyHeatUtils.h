#pragma once

#include <Arduino.h>

namespace MyHeatUtils
{
    String getConvertedStateToText(byte state);
    String getConvertedActiveToText(bool active);
    String getCurrentDateAndTime();
}