#ifndef MYHEATCUSTOMFUNCTIONS_H
#define MYHEATCUSTOMFUNCTIONS_H
#include <Arduino.h>
#include "MyHeatCustomFunction.h"
#include "MyHeatSave.h"
#include <LittleFS.h>

class MyHeatCustomFunctions : public MyHeatSaveInterface
{
private:
    MyHeatCustomFunction customFunctions[FUNCTION_COUNT];
    MyHeatSave *customFunctionsData;

    void serialize(JsonDocument &doc)
    {
        for (int i = 0; i < FUNCTION_COUNT; i++)
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
        for (int i = 0; i < FUNCTION_COUNT; i++)
        {
            customFunctions[i].setSign(doc[F("functions")][i][F("sign")]);
            customFunctions[i].setIsEnabled(doc[F("functions")][i][F("isEnabled")]);
            customFunctions[i].setTemperatureIndex(0, doc[F("functions")][i][F("temperatureIndex")][0]);
            customFunctions[i].setTemperatureIndex(1, doc[F("functions")][i][F("temperatureIndex")][1]);
            customFunctions[i].setDeltaValue(0, doc[F("functions")][i][F("deltaValue")][0]);
            customFunctions[i].setDeltaValue(1, doc[F("functions")][i][F("deltaValue")][1]);
            customFunctions[i].setRelayIndex(doc[F("functions")][i][F("relayIndex")]);
        }
    }

public:
    void begin()
    {
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

    void toggleCustomFunctionIsEnabled(byte functionIndex)
    {
        if(!customFunctions[functionIndex].isValid())
            return;

        customFunctions[functionIndex].toggleIsEnabled();
        save();
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

    void save()
    {
        customFunctionsData->save();
    }
};
#endif
