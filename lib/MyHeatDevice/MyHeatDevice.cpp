#include "MyHeatDevice.h"

void MyHeatDevice::begin()
{
    MyHeatTemperatures::begin();

    updateTemperature();
    MyHeatCustomFunctions::begin(this);
    MyHeatRelays::begin(getCustomFunctions());
    updateRelays();

    hardwareIO.begin(getTemperatures(), getRelays());
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
