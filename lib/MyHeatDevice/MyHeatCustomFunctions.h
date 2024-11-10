#pragma once
#include <Arduino.h>
#include "MyHeatCustomFunction.h"
#include "MyHeatSave.h"
#include <LittleFS.h>

class MyHeatCustomFunctions : public MyHeatSaveInterface
{
private:
    MyHeatCustomFunction customFunctions[FUNCTION_COUNT];
    MyHeatSave *customFunctionsData;
    float *temperatures;

    void serialize(JsonDocument &doc)
    {
        for (int i = 0; i < FUNCTION_COUNT; i++)
        {
            doc[F("functions")][i][F("sign")] = customFunctions[i].getSign();
            doc[F("functions")][i][F("isEnabled")] = customFunctions[i].getIsEnabled();
            doc[F("functions")][i][F("isActive")] = customFunctions[i].getIsActive();
            doc[F("functions")][i][F("temperatureIndex")][0] = customFunctions[i].getTemperatureIndex(0);
            doc[F("functions")][i][F("temperatureIndex")][1] = customFunctions[i].getTemperatureIndex(1);
            doc[F("functions")][i][F("deltaValue")][0] = customFunctions[i].getDeltaValue(0);
            doc[F("functions")][i][F("deltaValue")][1] = customFunctions[i].getDeltaValue(1);
            doc[F("functions")][i][F("relayIndex")] = customFunctions[i].getRelayIndex();
        }
    }

    void deserialize(JsonDocument &doc)
    {
        for (int i = 0; i < FUNCTION_COUNT; i++)
        {
            customFunctions[i].setSign(doc[F("functions")][i][F("sign")]);
            customFunctions[i].setIsEnabled(doc[F("functions")][i][F("isEnabled")]);
            customFunctions[i].setIsActive(doc[F("functions")][i][F("isActive")]);
            customFunctions[i].setTemperatureIndex(0, doc[F("functions")][i][F("temperatureIndex")][0]);
            customFunctions[i].setTemperatureIndex(1, doc[F("functions")][i][F("temperatureIndex")][1]);
            customFunctions[i].setDeltaValue(0, doc[F("functions")][i][F("deltaValue")][0]);
            customFunctions[i].setDeltaValue(1, doc[F("functions")][i][F("deltaValue")][1]);
            customFunctions[i].setRelayIndex(doc[F("functions")][i][F("relayIndex")]);
        }
    }

public:
    void begin(float *temperatures)
    {
        this->temperatures = temperatures;
        customFunctionsData = new MyHeatSave(&LittleFS, "/customFunctions.json", this);
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

    void setCustomFunctionIsEnabled(byte functionIndex)
    {
        customFunctions[functionIndex].toggleIsEnabled();
        customFunctionsData->save();
    }

    void setCustomFunctionSign(byte functionIndex, byte sign)
    {
        customFunctions[functionIndex].setSign(sign);
        customFunctionsData->save();
    }

    void setCustomFunctionTemperatureIndex(byte functionIndex, byte tempIndex, byte tempSensorIndex)
    {
        customFunctions[functionIndex].setTemperatureIndex(tempIndex, tempSensorIndex);
        customFunctionsData->save();
    }

    void setCustomFunctionDeltaValue(byte functionIndex, byte deltaValueIndex, float deltaValue)
    {
        customFunctions[functionIndex].setDeltaValue(deltaValueIndex, deltaValue);
        customFunctionsData->save();
    }

    void setCustomFunctionRelayIndex(byte functionIndex, byte relayIndex)
    {
        customFunctions[functionIndex].setRelayIndex(relayIndex);
        customFunctionsData->save();
    }

    void checkCustomFunctions()
    {
        for (int i = 0; i < FUNCTION_COUNT; i++)
        {
            float tempA = temperatures[customFunctions[i].getTemperatureIndex(0)] + customFunctions[i].getDeltaValue(0);
            float tempB = temperatures[customFunctions[i].getTemperatureIndex(1)] + customFunctions[i].getDeltaValue(1);

            if (!customFunctions[i].getIsEnabled())
            {
                customFunctions[i].setIsActive(false);
                continue;
            }

            if (customFunctions[i].getSign() == 0 && tempA < tempB)
            {
                customFunctions[i].setIsActive(true);
            }
            else if (customFunctions[i].getSign() == 1 && tempA == tempB)
            {
                customFunctions[i].setIsActive(true);
            }
            else if (customFunctions[i].getSign() == 2 && tempA > tempB)
            {
                customFunctions[i].setIsActive(true);
            }
            else
            {
                customFunctions[i].setIsActive(false);
            }
        }
    }
};
