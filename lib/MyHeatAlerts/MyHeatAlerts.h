#ifndef MYHEATALERTS_H
#define MYHEATALERTS_H

#include <Arduino.h>
#include <MyHeatWeb.h>
#include <MyHeatDevice.h>
#include <MyHeatTelebot.h>

namespace MyHeatAlerts
{
    // void begin();
    void tick();
    void checkTemperatureAlerts(bool forceNotification);
    void checkSmokeAlerts(bool forceNotification);
    void checkFunctionsAlerts(bool forceNotification);
}

#endif