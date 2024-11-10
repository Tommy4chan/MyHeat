#pragma once

#include <Arduino.h>

namespace MyHeatUtils
{
    String getConvertedStateToText(byte state);
    String getConvertedActiveToText(bool active);
    String getCurrentDateAndTime();
    String getAddressToString(uint8_t address[8]);
    bool isFloat(const String& str);
    bool isTimeDefault();
}