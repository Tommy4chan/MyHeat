#include "MyHeatDevice.h"

void MyHeatDevice::begin()
{
    customFunctionsData = new FileData(&LittleFS, "/customFunctions.dat", 'A', &customFunctions, sizeof(customFunctions));
    relaysData = new FileData(&LittleFS, "/relays.dat", 'B', &relays, sizeof(relays));

    readFileData(customFunctionsData);
    readFileData(relaysData);

    initRelays();
}

void MyHeatDevice::readFileData(FileData *fileData)
{
    FDstat_t stat = fileData->read();

    switch (stat)
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

void MyHeatDevice::initRelays()
{
    byte relayPinsArray[] = RELAY_PINS;

    for (int i = 0; i < RELAY_COUNT; i++)
    {
        relays[i].setPin(relayPinsArray[i]);
    }
}

MyHeatCustomFunction MyHeatDevice::getCustomFunction(byte index)
{
    return customFunctions[index];
}

MyHeatCustomFunction *MyHeatDevice::getCustomFunctions()
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

MyHeatRelay MyHeatDevice::getRelay(byte relayIndex)
{
    return relays[relayIndex];
}

MyHeatRelay *MyHeatDevice::getRelays()
{
    return relays;
}

void MyHeatDevice::changeRelayMode(byte relayIndex)
{
    relays[relayIndex].changeMode();
    relaysData->updateNow();
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

void MyHeatDevice::updateRelays()
{
    bool wasRelayActive[RELAY_COUNT] = {false};

    for (int i = 0; i < FUNCTION_COUNT; i++)
    {
        if (customFunctions[i].getIsActive() && !wasRelayActive[customFunctions[i].getRelayIndex()])
        {
            wasRelayActive[customFunctions[i].getRelayIndex()] = true;
        }
    }

    for (int i = 0; i < RELAY_COUNT; i++)
    {
        if (relays[i].getMode() == 0)
        {
            relays[i].setIsActive(false);
        }
        else if (relays[i].getMode() == 1)
        {
            relays[i].setIsActive(true);
        }
        else
        {
            relays[i].setIsActive(wasRelayActive[i]);
        }
    }
}

void MyHeatDevice::tick()
{
    if (millis() - tickTimer >= 5000)
    {
        tickTimer = millis();
        checkCustomFunctions();
        updateRelays();
    }
}
