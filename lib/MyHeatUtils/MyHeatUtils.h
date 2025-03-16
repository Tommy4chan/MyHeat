#ifndef MYHEATUTILS_H
#define MYHEATUTILS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>


namespace MyHeatUtils
{
    String getConvertedStateToText(byte state);
    String getConvertedActiveToText(bool active);
    String getCurrentDateAndTime();
    String getAddressToString(uint8_t address[8]);
    JsonDocument getFileContent(String path);
    bool isFloat(const String& str);
    bool isTimeDefault();
}

#endif
