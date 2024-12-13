#include "MyHeatDevice.h"

void MyHeatDevice::begin()
{
    MyHeatTemperatures::begin();

    updateTemperature();
    MyHeatCustomFunctions::begin();
    MyHeatRelays::begin();
    updateRelays();

    hardwareIO.begin(this, getRelays());
}

void MyHeatDevice::checkCustomFunctions()
{
    for (int i = 0; i < FUNCTION_COUNT; i++)
    {
        MyHeatCustomFunction customFunction = getCustomFunction(i);

        float tempA = getTemperature(customFunction.getTemperatureIndex(0)) + customFunction.getDeltaValue(0);
        float tempB = getTemperature(customFunction.getTemperatureIndex(1)) + customFunction.getDeltaValue(1);

        if (!customFunction.getIsEnabled())
        {
            customFunction.setIsActive(false);
            continue;
        }

        if (customFunction.getSign() == 0 && tempA < tempB)
        {
            customFunction.setIsActive(true);
        }
        else if (customFunction.getSign() == 1 && tempA == tempB)
        {
            customFunction.setIsActive(true);
        }
        else if (customFunction.getSign() == 2 && tempA > tempB)
        {
            customFunction.setIsActive(true);
        }
        else
        {
            customFunction.setIsActive(false);
        }
    }
}

void MyHeatDevice::updateRelays()
{
    bool isSetRelayActive[RELAY_COUNT] = {false};

    for (int i = 0; i < FUNCTION_COUNT; i++)
    {
        MyHeatCustomFunction customFunction = getCustomFunction(i);

        if (customFunction.getIsEnabled())
        {
            isSetRelayActive[customFunction.getRelayIndex()] = isSetRelayActive[customFunction.getRelayIndex()] || customFunction.getIsActive();
        }
    }

    for (int i = 0; i < RELAY_COUNT; i++)
    {
        MyHeatRelay relay = getRelay(i);

        if (relay.getMode() == 0)
        {
            relay.setIsActive(false);
        }
        else if (relay.getMode() == 1)
        {
            relay.setIsActive(true);
        }
        else
        {
            relay.setIsActive(isSetRelayActive[i]);
        }
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
        updateTemperature();
        updateRelays();
    }

    hardwareIO.tick();
}
