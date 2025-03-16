#ifndef MYHEATALERTS_H
#define MYHEATALERTS_H

#include <Arduino.h>
#include <MyHeatWeb.h>
#include <MyHeatDevice.h>
#include <MyHeatTelebot.h>

namespace MyHeatAlerts
{
    inline uint32_t lastCheckTick = -10000;

    // void begin();
    void tick();
    void checkTemperatureAlerts();
    void checkSmokeAlerts();
    // void checkFunctionsAlerts();
}

#endif