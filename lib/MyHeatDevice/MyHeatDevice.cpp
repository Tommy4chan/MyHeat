#include "MyHeatDevice.h"

void MyHeatDevice::begin()
{
    temperatures.begin();

    temperatures.updateTemperatures();
    customFunctions.begin();
    relays.begin();
    smokeSensor.begin();

    validateCustomFunctions();
    checkCustomFunctions();
    updateRelays();
}



void MyHeatDevice::validateCustomFunctions()
{
    std::vector<MyHeatCustomFunction>& cfArray = customFunctions.getCustomFunctions();

    for (int i = 0; i < customFunctions.getCustomFunctionCount(); i++)
    {
        bool isInvalid = false;

        customFunctions.resetFunctionAlert(i);

        if (cfArray[i].getTemperatureIndex(0) >= temperatures.getTemperatureCount() && cfArray[i].getTemperatureIndex(0) != TN_INDEX)
        {
            customFunctions.setFunctionAlert(i, FA_BAD_TEMPERATURE_NUMBER);
            cfArray[i].setTemperatureIndex(0, TEMP_UNKNOWN);
            isInvalid = true;
        }

        if (cfArray[i].getTemperatureIndex(1) >= temperatures.getTemperatureCount() && cfArray[i].getTemperatureIndex(1) != TN_INDEX)
        {
            customFunctions.setFunctionAlert(i, FA_BAD_TEMPERATURE_NUMBER);
            cfArray[i].setTemperatureIndex(1, TEMP_UNKNOWN);
            isInvalid = true;
        }

        if (cfArray[i].getRelayIndex() >= relays.getRelayCount())
        {
            customFunctions.setFunctionAlert(i, FA_BAD_RELAY_NUMBER);
            cfArray[i].setRelayIndex(RELAY_UNKNOWN);
            isInvalid = true;
        }

        if (isInvalid)
        {
            cfArray[i].setIsEnabled(false);
        }
    }

    customFunctions.save();
}

void MyHeatDevice::checkCustomFunctions()
{
    std::vector<MyHeatCustomFunction>& cfArray = customFunctions.getCustomFunctions();

    for (int i = 0; i < customFunctions.getCustomFunctionCount(); i++)
    {
        if (!cfArray[i].getIsEnabled())
        {
            cfArray[i].setIsActive(false);
            continue;
        }

        float tempA = temperatures.getTemperature(cfArray[i].getTemperatureIndex(0));
        float tempB = temperatures.getTemperature(cfArray[i].getTemperatureIndex(1));

        if (tempA <= TEMPERATURE_ERROR + 0.1 || tempB <= TEMPERATURE_ERROR + 0.1)
        {
            customFunctions.setFunctionAlert(i, FA_BAD_TEMPERATURE);
            cfArray[i].setIsActive(false);
            continue;
        }

        customFunctions.resetFunctionAlert(i);

        tempA += cfArray[i].getDeltaValue(0);
        tempB += cfArray[i].getDeltaValue(1);

        if (cfArray[i].getSign() == CustomFunctionSign::LESS && tempA < tempB)
        {
            cfArray[i].setIsActive(true);
        }
        else if (cfArray[i].getSign() == CustomFunctionSign::EQUAL && abs(tempA - tempB) < 0.1)
        {
            cfArray[i].setIsActive(true);
        }
        else if (cfArray[i].getSign() == CustomFunctionSign::GREATER && tempA > tempB)
        {
            cfArray[i].setIsActive(true);
        }
        else
        {
            cfArray[i].setIsActive(false);
        }
    }
}

void MyHeatDevice::updateRelays()
{
    std::vector<MyHeatCustomFunction>& cfArray = customFunctions.getCustomFunctions();
    std::vector<MyHeatRelay>& relaysArray = relays.getRelays();

    std::vector<bool> activeRelays(relays.getRelayCount(), false);

    for (int i = 0; i < customFunctions.getCustomFunctionCount(); i++)
    {
        if (cfArray[i].getIsEnabled())
        {
            byte relayIdx = cfArray[i].getRelayIndex();
            if (relayIdx < activeRelays.size()) {
                activeRelays[relayIdx] = activeRelays[relayIdx] || cfArray[i].getIsActive();
            }
        }
    }

    for (int i = 0; i < relays.getRelayCount(); i++)
    {
        if (relaysArray[i].getMode() == RelayMode::OFF)
        {
            relaysArray[i].setIsActive(false);
        }
        else if (relaysArray[i].getMode() == RelayMode::ON)
        {
            relaysArray[i].setIsActive(true);
        }
        else
        {
            relaysArray[i].setIsActive(activeRelays[i]);
        }
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

    if (millis() - tickTimerSecondary >= 750)
    {
        tickTimerSecondary = millis();
        temperatures.updateTemperatures();
        updateRelays();
        smokeSensor.updateSmokeSensor();
    }
}
