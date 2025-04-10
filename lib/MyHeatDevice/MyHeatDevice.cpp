#include "MyHeatDevice.h"

void MyHeatDevice::begin()
{
    MyHeatTemperatures::begin();

    updateTemperatures();
    MyHeatCustomFunctions::begin();
    MyHeatRelays::begin();
    MyHeatSmokeSensor::begin();

    initIsSetRelayActive(); 

    validateCustomFunctions();
    checkCustomFunctions();
    updateRelays();
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

    for (int i = 0; i < getCustomFunctionCount(); i++)
    {
        bool isInvalid = false;

        resetFunctionAlert(i);

        if (customFunctions[i].getTemperatureIndex(0) >= getTemperatureCount() && customFunctions[i].getTemperatureIndex(0) != TN_INDEX)
        {
            setFunctionAlert(i, FA_BAD_TEMPERATURE_NUMBER);
            customFunctions[i].setTemperatureIndex(0, TEMP_UNKNOWN);
            isInvalid = true;
        }

        if (customFunctions[i].getTemperatureIndex(1) >= getTemperatureCount() && customFunctions[i].getTemperatureIndex(1) != TN_INDEX)
        {
            setFunctionAlert(i, FA_BAD_TEMPERATURE_NUMBER);
            customFunctions[i].setTemperatureIndex(1, TEMP_UNKNOWN);
            isInvalid = true;
        }

        if (customFunctions[i].getRelayIndex() >= getRelayCount())
        {
            setFunctionAlert(i, FA_BAD_RELAY_NUMBER);
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

void MyHeatDevice::checkCustomFunctions()
{
    MyHeatCustomFunction *customFunctions = getCustomFunctions();

    for (int i = 0; i < getCustomFunctionCount(); i++)
    {
        if (!customFunctions[i].getIsEnabled())
        {
            customFunctions[i].setIsActive(false);
            continue;
        }

        float tempA = getTemperature(customFunctions[i].getTemperatureIndex(0));
        float tempB = getTemperature(customFunctions[i].getTemperatureIndex(1));

        if (tempA == TEMPERATURE_ERROR || tempB == TEMPERATURE_ERROR)
        {
            setFunctionAlert(i, FA_BAD_TEMPERATURE);
            customFunctions[i].setIsActive(false);
            continue;
        }

        resetFunctionAlert(i);

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

    for (int i = 0; i < getCustomFunctionCount(); i++)
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

void MyHeatDevice::manualTick()
{
    tickTimerMain = millis();
    tickTimerSecondary = millis();
    checkCustomFunctions();
    updateRelays();
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
        updateSmokeSensor();
    }
}
