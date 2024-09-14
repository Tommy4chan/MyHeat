#include "MyHeatDevice.h"

void MyHeatDevice::begin() {
    customFunctionsData = new FileData(&LittleFS, "/customFunctions.dat", 'A', &customFunctions, sizeof(customFunctions));

    FDstat_t customFunctionsStat = customFunctionsData->read();

    switch (customFunctionsStat)
    {
    case FD_FS_ERR:
        Serial.println("FS Error");
        break;
    case FD_FILE_ERR:
        Serial.println("Error");
        break;
    case FD_WRITE:
        Serial.println("Data Write");
        break;
    case FD_ADD:
        Serial.println("Data Add");
        break;
    case FD_READ:
        Serial.println("Data Read");
        break;
    default:
        break;
    }
}

CustomFunction MyHeatDevice::getCustomFunction(byte index)
{
    return customFunctions[index];
}

CustomFunction *MyHeatDevice::getCustomFunctions()
{
    return customFunctions;
}

void MyHeatDevice::setCustomFunctionIsEnabled(byte functionIndex)
{
    customFunctions[functionIndex].toggleIsEnabled();
    customFunctionsData->updateNow();
}

void MyHeatDevice::setCustomFunctionSign(byte functionIndex, byte sign)
{
    customFunctions[functionIndex].setSign(sign);
    customFunctionsData->updateNow();
}

void MyHeatDevice::setCustomFunctionTemperatureIndex(byte functionIndex, byte tempIndex, byte tempSensorIndex)
{
    customFunctions[functionIndex].setTemperatureIndex(tempIndex, tempSensorIndex);
    customFunctionsData->updateNow();
}

void MyHeatDevice::setCustomFunctionDeltaValue(byte functionIndex, byte deltaValueIndex, float deltaValue)
{
    customFunctions[functionIndex].setDeltaValue(deltaValueIndex, deltaValue);
    customFunctionsData->updateNow();
}

void MyHeatDevice::setCustomFunctionRelayIndex(byte functionIndex, byte relayIndex)
{
    customFunctions[functionIndex].setRelayIndex(relayIndex);
    customFunctionsData->updateNow();
}

void MyHeatDevice::checkCustomFunctions()
{
    float temperatures[] = {0, 10, 20, 30, 40, 50, 0};

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

void MyHeatDevice::tick()
{
    if (millis() - tickTimer >= 5000)
    {
        tickTimer = millis();
        checkCustomFunctions();
    }
}
