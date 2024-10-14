#include "MyHeatDevice.h"

void MyHeatDevice::begin()
{
    customFunctionsData = new FileData(&LittleFS, "/customFunctions.dat", 'A', &customFunctions, sizeof(customFunctions));
    relaysData = new FileData(&LittleFS, "/relays.dat", 'B', &relays, sizeof(relays));
    temperatureSensorData = new FileData(&LittleFS, "/temperatures.dat", 'C', &temperatureSensorsAddresses, sizeof(temperatureSensorsAddresses));

    readFileData(customFunctionsData);
    readFileData(relaysData);
    readFileData(temperatureSensorData);

    initRelays();
    hardwareIO.begin();
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
    bool isSetRelayActive[RELAY_COUNT] = {false};

    for (int i = 0; i < FUNCTION_COUNT; i++)
    {
        if (customFunctions[i].getIsEnabled())
        {
            isSetRelayActive[customFunctions[i].getRelayIndex()] = isSetRelayActive[customFunctions[i].getRelayIndex()] && customFunctions[i].getIsActive();
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
            relays[i].setIsActive(isSetRelayActive[i]);
        }
    }
}

byte MyHeatDevice::discoverTemperatureSensor()
{
    uint8_t address[8];
    byte count = 0;

    while (oneWire.search(address))
    {
        bool flag = false;
        for (int i = 0; i < TEMPERATURE_COUNT; i++)
        {
            if (memcmp(temperatureSensorsAddresses[i], address, 8) == 0)
            {
                flag = true;
                break;
            }
        }

        if (flag)
            continue;
        memcpy(discoveredTemperatureSensorsAddresses[count], address, 8);
        count++;
    }

    return count;
}

void MyHeatDevice::setTemperatureSensorAddress(byte tempIndex, byte sensorAddressIndex)
{
    memcpy(temperatureSensorsAddresses[tempIndex], discoveredTemperatureSensorsAddresses[sensorAddressIndex], 8);
    temperatureSensorData->updateNow();
}

void MyHeatDevice::deleteTemperatureSensorAddress(byte tempIndex)
{
    memset(temperatureSensorsAddresses[tempIndex], 0, 8);
    temperatureSensorData->updateNow();
}

uint8_t (*MyHeatDevice::getDiscoveredTemperatureSensorAddresses())[8]
{
    return discoveredTemperatureSensorsAddresses;
}

uint8_t (*MyHeatDevice::getTemperatureSensorAddresses())[8]
{
    return temperatureSensorsAddresses;
}

void MyHeatDevice::updateTemperature()
{
    for (byte i = 0; i < TEMPERATURE_COUNT; i++)
    {
        temperatures[i] = temperatureSensors.getTempC(temperatureSensorsAddresses[i]);
    }
    temperatureSensors.requestTemperatures();
}

float *MyHeatDevice::getTemperatures()
{
    return temperatures;
}

void MyHeatDevice::tick()
{
    if (millis() - tickTimerMain >= 5000)
    {
        tickTimerMain = millis();
        checkCustomFunctions();
        updateRelays();
    }

    if (millis() - tickTimerSecondary >= 1000)
    {
        tickTimerSecondary = millis();
        updateTemperature();
    }

    hardwareIO.tick();
}
