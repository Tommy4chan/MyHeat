#pragma once
#include <Arduino.h>
#include <FileData.h>
#include <LittleFS.h>
#include "CustomFunction.h"

// #include <FS.h>

// Створити все в цьому неймспейсі
// Використовувати цю лібу https://github.com/GyverLibs/GyverDS18 і цей клас GyverDS18

class MyHeatDevice
{
private:
    CustomFunction customFunctions[FUNCTION_COUNT];
    uint32_t tickTimer;
    FileData *customFunctionsData;

public:
    void begin();
    CustomFunction getCustomFunction(byte index);
    CustomFunction *getCustomFunctions();
    void setCustomFunctionIsEnabled(byte functionIndex);
    void setCustomFunctionSign(byte functionIndex, byte sign);
    void setCustomFunctionTemperatureIndex(byte functionIndex, byte tempIndex, byte tempSensorIndex);
    void setCustomFunctionDeltaValue(byte functionIndex, byte deltaValueIndex, float deltaValue);
    void setCustomFunctionRelayIndex(byte functionIndex, byte relayIndex);
    void checkCustomFunctions();
    void tick();
};