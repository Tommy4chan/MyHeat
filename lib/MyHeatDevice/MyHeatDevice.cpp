#include "MyHeatDevice.h"

void MyHeatDevice::begin()
{
    MyHeatTemperatures::begin();

    updateTemperatures();
    MyHeatCustomFunctions::begin();
    MyHeatRelays::begin();

    initIsSetRelayActive(); 

    validateCustomFunctions();
    checkCustomFunctions();
    updateRelays();

    hardwareIO.begin(this, this);
}

void MyHeatDevice::initIsSetRelayActive() 
{
    byte size = getRelayCount();
    delete[] isSetRelayActive;

    isSetRelayActive = new bool[size];
    for (int i = 0; i < size; i++)
    {
        isSetRelayActive[i] = false;
    }
}

void MyHeatDevice::validateCustomFunctions()
{
    MyHeatCustomFunction *customFunctions = getCustomFunctions();

    for (int i = 0; i < FUNCTION_COUNT; i++)
    {
        bool isInvalid = false;

        if (customFunctions[i].getTemperatureIndex(0) >= getTemperatureCount() && customFunctions[i].getTemperatureIndex(0) != TN_INDEX)
        {
            customFunctions[i].setTemperatureIndex(0, T_UNKNOWN);
            isInvalid = true;
        }

        if (customFunctions[i].getTemperatureIndex(1) >= getTemperatureCount() && customFunctions[i].getTemperatureIndex(1) != TN_INDEX)
        {
            customFunctions[i].setTemperatureIndex(1, T_UNKNOWN);
            isInvalid = true;
        }

        if (customFunctions[i].getRelayIndex() >= getRelayCount())
        {
            customFunctions[i].setRelayIndex(RELAY_UNKNOWN);
            isInvalid = true;
        }

        if (isInvalid)
        {
            customFunctions[i].setIsEnabled(false);
        }
    }

    MyHeatCustomFunctions::save();
}

void MyHeatDevice::updateTemperatureSensorsSettings(byte pin, byte count)
{
    setTemperaturePin(pin);
    setTemperatureCount(count);
    MyHeatTemperatures::save();

    hardwareIO.reevaluateScreensCount();
    validateCustomFunctions();
}

void MyHeatDevice::updateRelayCount(byte count)
{
    setRelayCount(count);

    initIsSetRelayActive();
    hardwareIO.reevaluateScreensCount();
    validateCustomFunctions();
}

void MyHeatDevice::updateRelaysSettings(JsonObject payload)
{
    for (int i = 0; i < getRelayCount(); i++)
    {
        setRelaySettings(i, payload["relays"][i]["pin"], payload["relays"][i]["isActiveOnHigh"]);
    }

    MyHeatRelays::save();
}

void MyHeatDevice::checkCustomFunctions()
{
    MyHeatCustomFunction *customFunctions = getCustomFunctions();

    for (int i = 0; i < FUNCTION_COUNT; i++)
    {
        if (!customFunctions[i].getIsEnabled())
        {
            customFunctions[i].setIsActive(false);
            continue;
        }

        float tempA = getTemperature(customFunctions[i].getTemperatureIndex(0));
        float tempB = getTemperature(customFunctions[i].getTemperatureIndex(1));

        if (tempA == -127.00 || tempB == -127.00)
        {
            customFunctions[i].setIsActive(false);
            continue;
        }

        tempA += customFunctions[i].getDeltaValue(0);
        tempB += customFunctions[i].getDeltaValue(1);

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
    MyHeatCustomFunction *customFunctions = getCustomFunctions();
    MyHeatRelay *relays = getRelays();

    for (int i = 0; i < FUNCTION_COUNT; i++)
    {
        if (customFunctions[i].getIsEnabled())
        {
            isSetRelayActive[customFunctions[i].getRelayIndex()] = isSetRelayActive[customFunctions[i].getRelayIndex()] || customFunctions[i].getIsActive();
        }
    }

    for (int i = 0; i < getRelayCount(); i++)
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

        isSetRelayActive[i] = false;
    }
}

void MyHeatDevice::tick()
{
    if (millis() - tickTimerMain >= 5000)
    {
        tickTimerMain = millis();
        checkCustomFunctions();
    }

    if (millis() - tickTimerSecondary >= 1000)
    {
        tickTimerSecondary = millis();
        updateTemperatures();
        updateRelays();
    }

    hardwareIO.tick();
}
