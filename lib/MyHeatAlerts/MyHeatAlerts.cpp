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
        checkFunctionsAlerts();
        // lastCheckTick = millis();
    }

    void checkTemperatureAlerts()
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (byte i = 0; i < myHeatDevice.getTemperatureCount(); i++)
        {
            if (myHeatDevice.isTemperatureSensorAddressEmpty(i))
            {
                continue;
            }

            String message = "";

            TemperatureAlert alert = myHeatDevice.getTemperatureAlert(i);
            if (alert == TA_NONE)
            {
                continue;
            }
            else if (alert == TA_MIN)
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

            MyHeatWeb::sendAlertNotification(message);
            MyHeatTelebot::sendAlertNotification(message);
        }
    }

    void checkSmokeAlerts()
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        if (myHeatDevice.MyHeatSmokeSensor::getIsSendSmokeSensorNotification())
        {
            MyHeatWeb::sendAlertNotification("Димовий датчик виявив дим");
            MyHeatTelebot::sendAlertNotification("Димовий датчик виявив дим");
        }
    }

    void checkFunctionsAlerts() {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (byte i = 0; i < myHeatDevice.getCustomFunctionCount(); i++)
        {
            FunctionAlert alert = myHeatDevice.getFunctionAlert(i);

            String message = "";

            if (alert == FA_NONE)
            {
                continue;
            }
            else if (alert == FunctionAlert::FA_BAD_TEMPERATURE)
            {
                message = "Функція " + String(i) + " не може бути активною через проблеми з датчиками температури";
            }

            MyHeatWeb::sendAlertNotification(message);
            MyHeatTelebot::sendAlertNotification(message);
        }
    }

}