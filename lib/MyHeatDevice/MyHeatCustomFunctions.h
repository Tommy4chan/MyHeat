#ifndef MYHEATCUSTOMFUNCTIONS_H
#define MYHEATCUSTOMFUNCTIONS_H
#include <Arduino.h>
#include "MyHeatCustomFunction.h"
#include "MyHeatSave.h"
#include <LittleFS.h>

enum FunctionAlert
{
    FA_NONE = 0,
    FA_BAD_TEMPERATURE = 1,
    FA_BAD_TEMPERATURE_NUMBER = 2,
    FA_BAD_RELAY_NUMBER = 3,
};

class MyHeatCustomFunctions : public MyHeatSaveInterface
{
private:
    MyHeatCustomFunction *customFunctions;
    MyHeatSave *customFunctionsData;
    FunctionAlert *functionAlerts;

    byte functionCount;

    void serialize(JsonDocument &doc);
    void deserialize(JsonDocument &doc);
    void realocateMemory(byte newFunctionCount);

public:
    MyHeatCustomFunctions();
    void begin();
    MyHeatCustomFunction getCustomFunction(byte index);
    MyHeatCustomFunction *getCustomFunctions();
    void toggleCustomFunctionIsEnabled(byte functionIndex);
    bool setCustomFunctionIsEnabled(byte functionIndex, bool isEnabled);
    void setCustomFunctionSign(byte functionIndex, CustomFunctionSign sign);
    void setCustomFunctionTemperatureIndex(byte functionIndex, byte tempIndex, byte tempSensorIndex);
    void setCustomFunctionDeltaValue(byte functionIndex, byte deltaValueIndex, float deltaValue);
    void setCustomFunctionRelayIndex(byte functionIndex, byte relayIndex);
    void setCustomFunctionCount(byte newCustomFunctionCount);
    byte getCustomFunctionCount();
    void manualDeserialize(JsonDocument payload);
    void setFunctionAlert(byte functionIndex, FunctionAlert alert);
    void resetFunctionAlert(byte functionIndex);
    FunctionAlert getFunctionAlert(byte functionIndex);
    void save();
};

#endif
