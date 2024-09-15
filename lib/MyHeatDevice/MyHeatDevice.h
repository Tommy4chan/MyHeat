#pragma once
#include <Arduino.h>
#include <FileData.h>
#include <LittleFS.h>
#include "MyHeatCustomFunction.h"
#include "MyHeatRelay.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

// #include <FS.h>

// Створити все в цьому неймспейсі
// Використовувати цю лібу https://github.com/GyverLibs/GyverDS18 і цей клас GyverDS18

class MyHeatDevice
{
private:
    MyHeatCustomFunction customFunctions[FUNCTION_COUNT];
    MyHeatRelay relays[RELAY_COUNT];
    uint32_t tickTimer;
    FileData *customFunctionsData;
    FileData *relaysData;
    void readFileData(FileData *fileData);
    void checkCustomFunctions();
    void updateRelays();

public:
    void begin();
    void initRelays();
    MyHeatCustomFunction getCustomFunction(byte index);
    MyHeatCustomFunction *getCustomFunctions();
    void setCustomFunctionIsEnabled(byte functionIndex);
    void setCustomFunctionSign(byte functionIndex, byte sign);
    void setCustomFunctionTemperatureIndex(byte functionIndex, byte tempIndex, byte tempSensorIndex);
    void setCustomFunctionDeltaValue(byte functionIndex, byte deltaValueIndex, float deltaValue);
    void setCustomFunctionRelayIndex(byte functionIndex, byte relayIndex);
    
    MyHeatRelay getRelay(byte relayIndex);
    MyHeatRelay *getRelays();
    void changeRelayMode(byte relayIndex);

    void tick();
};