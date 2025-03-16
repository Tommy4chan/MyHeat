#include "MyHeatAlerts.h"

namespace MyHeatAlerts
{
    void tick()
    {
        if (millis() - lastCheckTick < 5000)
        {
            return;
        }

        checkTemperatureAlerts();
        checkSmokeAlerts();
        lastCheckTick = millis();
    }

    void checkTemperatureAlerts()
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        myHeatDevice.checkForAlerts();

        for (byte i = 0; i < myHeatDevice.getTemperatureCount(); i++)
        {
            
            if (myHeatDevice.isTemperatureSensorAddressEmpty(i))
            {
                continue;
            }

            String message = "none123";

            TemperatureAlerts alert = myHeatDevice.getTemperatureAlert(i);

            if (alert == TA_MIN)
            {
                message = "Датчик температури Т" + String(i) + " має значення нижче мінімальної температури";
            }
            else if (alert == TA_MAX)
            {
                message = "Датчик температури Т" + String(i) + " має значення вище максимальної температури";
            }
            else if (alert == TA_BAD_CONNECTION)
            {
                message = "Датчик температури Т" + String(i) + " відключився";
            }

            if (alert != TA_NONE)
            {
                MyHeatWeb::sendAlertNotification(message);
                // MyHeatTelebot::sendAlertNotification(message);
            }
        }
    }

    void checkSmokeAlerts()
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        if (myHeatDevice.MyHeatSmokeSensor::getIsSendSmokeSensorNotification())
        {
            MyHeatWeb::sendAlertNotification("Димовий датчик виявив дим");
            // MyHeatTelebot::sendAlertNotification("Димовий датчик виявив дим");
        }
    }

}