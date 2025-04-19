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
    bool *isNotified;

    byte functionCount;

    void serialize(JsonDocument &doc)
    {
        doc[F("functionCount")] = functionCount;

        for (int i = 0; i < functionCount; i++)
        {
            doc[F("functions")][i][F("sign")] = customFunctions[i].getSign();
            doc[F("functions")][i][F("isEnabled")] = customFunctions[i].getIsEnabled();
            doc[F("functions")][i][F("temperatureIndex")][0] = customFunctions[i].getTemperatureIndex(0);
            doc[F("functions")][i][F("temperatureIndex")][1] = customFunctions[i].getTemperatureIndex(1);
            doc[F("functions")][i][F("deltaValue")][0] = customFunctions[i].getDeltaValue(0);
            doc[F("functions")][i][F("deltaValue")][1] = customFunctions[i].getDeltaValue(1);
            doc[F("functions")][i][F("relayIndex")] = customFunctions[i].getRelayIndex();
        }
    }

    void deserialize(JsonDocument &doc)
    {
        realocateMemory(doc[F("functionCount")] | FUNCTION_COUNT);

        for (int i = 0; i < functionCount; i++)
        {
            customFunctions[i].setSign(doc[F("functions")][i][F("sign")] | 0);
            customFunctions[i].setIsEnabled(doc[F("functions")][i][F("isEnabled")] | false);
            customFunctions[i].setTemperatureIndex(0, doc[F("functions")][i][F("temperatureIndex")][0] | 0);
            customFunctions[i].setTemperatureIndex(1, doc[F("functions")][i][F("temperatureIndex")][1] | 0);
            customFunctions[i].setDeltaValue(0, doc[F("functions")][i][F("deltaValue")][0] | 0);
            customFunctions[i].setDeltaValue(1, doc[F("functions")][i][F("deltaValue")][1] | 0);
            customFunctions[i].setRelayIndex(doc[F("functions")][i][F("relayIndex")] | 0);
        }
    }

    void realocateMemory(byte newFunctionCount)
    {
        MyHeatCustomFunction *oldFunctions = nullptr;
        if (customFunctions != nullptr)
        {
            oldFunctions = new MyHeatCustomFunction[functionCount];
            for (byte i = 0; i < functionCount; i++)
            {
                oldFunctions[i] = customFunctions[i];
            }
        }

        delete[] customFunctions;

        customFunctions = new MyHeatCustomFunction[newFunctionCount];
        functionAlerts = new FunctionAlert[newFunctionCount];
        isNotified = new bool[newFunctionCount];

        if (oldFunctions != nullptr)
        {
            byte copyCount = min(functionCount, newFunctionCount);
            for (byte i = 0; i < copyCount; i++)
            {
                customFunctions[i] = oldFunctions[i];
                functionAlerts[i] = FunctionAlert::FA_NONE;
                isNotified[i] = false;
            }

            delete[] oldFunctions;
        }

        functionCount = newFunctionCount;
    }

public:
    MyHeatCustomFunctions()
    {
        functionCount = 0;
        customFunctions = nullptr;
        realocateMemory(FUNCTION_COUNT);
    }

    void begin()
    {
        customFunctionsData = new MyHeatSave("/customFunctions.json", this);
        customFunctionsData->read();
    }

    MyHeatCustomFunction getCustomFunction(byte index)
    {
        return customFunctions[index];
    }

    MyHeatCustomFunction *getCustomFunctions()
    {
        return customFunctions;
    }

    void toggleCustomFunctionIsEnabled(byte functionIndex)
    {
        if (!customFunctions[functionIndex].isValid())
            return;

        customFunctions[functionIndex].toggleIsEnabled();
        save();
    }

    bool setCustomFunctionIsEnabled(byte functionIndex, bool isEnabled)
    {
        if (!customFunctions[functionIndex].isValid())
            return false;

        customFunctions[functionIndex].setIsEnabled(isEnabled);
        save();
        
        return true;
    }

    void setCustomFunctionSign(byte functionIndex, byte sign)
    {
        customFunctions[functionIndex].setSign(sign);
        save();
    }

    void setCustomFunctionTemperatureIndex(byte functionIndex, byte tempIndex, byte tempSensorIndex)
    {
        customFunctions[functionIndex].setTemperatureIndex(tempIndex, tempSensorIndex);
        save();
    }

    void setCustomFunctionDeltaValue(byte functionIndex, byte deltaValueIndex, float deltaValue)
    {
        customFunctions[functionIndex].setDeltaValue(deltaValueIndex, deltaValue);
        save();
    }

    void setCustomFunctionRelayIndex(byte functionIndex, byte relayIndex)
    {
        customFunctions[functionIndex].setRelayIndex(relayIndex);
        save();
    }

    void setCustomFunctionCount(byte newCustomFunctionCount)
    {
        realocateMemory(newCustomFunctionCount);
        save();
    }

    byte getCustomFunctionCount()
    {
        return functionCount;
    }

    void manualDeserialize(JsonDocument payload)
    {
        deserialize(payload);
        save();
    }

    void setFunctionAlert(byte functionIndex, FunctionAlert alert)
    {
        functionAlerts[functionIndex] = alert;
    }

    void resetFunctionAlert(byte functionIndex)
    {
        functionAlerts[functionIndex] = FunctionAlert::FA_NONE;
        isNotified[functionIndex] = false;
    }

    FunctionAlert getFunctionAlert(byte functionIndex)
    {
        if (functionAlerts[functionIndex] != FunctionAlert::FA_NONE && !isNotified[functionIndex])
        {
            isNotified[functionIndex] = true;
            return functionAlerts[functionIndex];
        }

        return FunctionAlert::FA_NONE;
    }

    void save()
    {
        customFunctionsData->save();
    }
};

#endif
