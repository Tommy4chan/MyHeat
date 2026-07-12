#include "MyHeatCustomFunctions.h"
#include <MyHeatAlerts.h>

void MyHeatCustomFunctions::serialize(JsonDocument &doc)
{
    doc[F("functionCount")] = functionCount;

    for (int i = 0; i < functionCount; i++)
    {
        doc[F("functions")][i][F("sign")] = static_cast<byte>(customFunctions[i].getSign());
        doc[F("functions")][i][F("isEnabled")] = customFunctions[i].getIsEnabled();
        doc[F("functions")][i][F("temperatureIndex")][0] = customFunctions[i].getTemperatureIndex(0);
        doc[F("functions")][i][F("temperatureIndex")][1] = customFunctions[i].getTemperatureIndex(1);
        doc[F("functions")][i][F("deltaValue")][0] = customFunctions[i].getDeltaValue(0);
        doc[F("functions")][i][F("deltaValue")][1] = customFunctions[i].getDeltaValue(1);
        doc[F("functions")][i][F("relayIndex")] = customFunctions[i].getRelayIndex();
    }
}

void MyHeatCustomFunctions::deserialize(JsonDocument &doc)
{
    realocateMemory(doc[F("functionCount")] | FUNCTION_COUNT);

    for (int i = 0; i < functionCount; i++)
    {
        customFunctions[i].setSign(static_cast<CustomFunctionSign>(doc[F("functions")][i][F("sign")] | 0));
        customFunctions[i].setIsEnabled(doc[F("functions")][i][F("isEnabled")] | false);
        customFunctions[i].setTemperatureIndex(0, doc[F("functions")][i][F("temperatureIndex")][0] | 0);
        customFunctions[i].setTemperatureIndex(1, doc[F("functions")][i][F("temperatureIndex")][1] | 0);
        customFunctions[i].setDeltaValue(0, doc[F("functions")][i][F("deltaValue")][0] | 0);
        customFunctions[i].setDeltaValue(1, doc[F("functions")][i][F("deltaValue")][1] | 0);
        customFunctions[i].setRelayIndex(doc[F("functions")][i][F("relayIndex")] | 0);
    }
}

void MyHeatCustomFunctions::realocateMemory(byte newFunctionCount)
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
    delete[] functionAlerts;

    customFunctions = new MyHeatCustomFunction[newFunctionCount];
    functionAlerts = new FunctionAlert[newFunctionCount];

    if (oldFunctions != nullptr)
    {
        byte copyCount = min(functionCount, newFunctionCount);
        for (byte i = 0; i < copyCount; i++)
        {
            customFunctions[i] = oldFunctions[i];
            functionAlerts[i] = FunctionAlert::FA_NONE;
        }

        delete[] oldFunctions;
    }

    functionCount = newFunctionCount;
}

MyHeatCustomFunctions::MyHeatCustomFunctions()
{
    functionCount = 0;
    customFunctions = nullptr;
    functionAlerts = nullptr;
    realocateMemory(FUNCTION_COUNT);
}

void MyHeatCustomFunctions::begin()
{
    customFunctionsData = new MyHeatSave("/customFunctions.json", this);
    customFunctionsData->read();
}

MyHeatCustomFunction MyHeatCustomFunctions::getCustomFunction(byte index)
{
    return customFunctions[index];
}

MyHeatCustomFunction *MyHeatCustomFunctions::getCustomFunctions()
{
    return customFunctions;
}

void MyHeatCustomFunctions::toggleCustomFunctionIsEnabled(byte functionIndex)
{
    if (!customFunctions[functionIndex].isValid())
        return;

    customFunctions[functionIndex].toggleIsEnabled();
    save();
}

bool MyHeatCustomFunctions::setCustomFunctionIsEnabled(byte functionIndex, bool isEnabled)
{
    if (!customFunctions[functionIndex].isValid())
        return false;

    customFunctions[functionIndex].setIsEnabled(isEnabled);
    save();
    
    return true;
}

void MyHeatCustomFunctions::setCustomFunctionSign(byte functionIndex, CustomFunctionSign sign)
{
    customFunctions[functionIndex].setSign(sign);
    save();
}

void MyHeatCustomFunctions::setCustomFunctionTemperatureIndex(byte functionIndex, byte tempIndex, byte tempSensorIndex)
{   
    customFunctions[functionIndex].setTemperatureIndex(tempIndex, tempSensorIndex);
    save();
}

void MyHeatCustomFunctions::setCustomFunctionDeltaValue(byte functionIndex, byte deltaValueIndex, float deltaValue)
{
    customFunctions[functionIndex].setDeltaValue(deltaValueIndex, deltaValue);
    save();
}

void MyHeatCustomFunctions::setCustomFunctionRelayIndex(byte functionIndex, byte relayIndex)
{
    customFunctions[functionIndex].setRelayIndex(relayIndex);
    save();
}

void MyHeatCustomFunctions::setCustomFunctionCount(byte newCustomFunctionCount)
{
    realocateMemory(newCustomFunctionCount);
    save();
}

byte MyHeatCustomFunctions::getCustomFunctionCount()
{
    return functionCount;
}

void MyHeatCustomFunctions::manualDeserialize(JsonDocument payload)
{
    deserialize(payload);
    save();
}

void MyHeatCustomFunctions::setFunctionAlert(byte functionIndex, FunctionAlert alert)
{
    if (functionAlerts[functionIndex] != alert)
    {
        functionAlerts[functionIndex] = alert;

        if (alert != FunctionAlert::FA_NONE)
        {
            String message = "";
            if (alert == FunctionAlert::FA_BAD_TEMPERATURE)
            {
                message = "Функція " + String(functionIndex) + " не може бути активною через проблеми з датчиками температури";
            }
            else if (alert == FunctionAlert::FA_BAD_TEMPERATURE_NUMBER)
            {
                message = "Функція " + String(functionIndex) + " не може бути активною через використання неіснуйочуго датчика температури";
            }
            else if (alert == FunctionAlert::FA_BAD_RELAY_NUMBER)
            {
                message = "Функція " + String(functionIndex) + " не може бути активною через використання неіснуйочуго реле";
            }

            MyHeatAlerts::triggerAlert(message);
        }
    }
}

void MyHeatCustomFunctions::resetFunctionAlert(byte functionIndex)
{
    functionAlerts[functionIndex] = FunctionAlert::FA_NONE;
}

FunctionAlert MyHeatCustomFunctions::getFunctionAlert(byte functionIndex)
{
    return functionAlerts[functionIndex];
}

void MyHeatCustomFunctions::save()
{
    customFunctionsData->save();
}
