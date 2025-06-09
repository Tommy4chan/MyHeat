#include "MyHeatAlerts.h"

namespace MyHeatAlerts
{
    void tick()
    {
        checkTemperatureAlerts(false);
        checkSmokeAlerts(false);
        checkFunctionsAlerts(false);
    }

    void checkTemperatureAlerts(bool forceNotification)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        bool manualTick = false;

        for (byte i = 0; i < myHeatDevice.getTemperatureCount(); i++)
        {
            if (myHeatDevice.isTemperatureSensorAddressEmpty(i))
            {
                continue;
            }

            String message = "";

            TemperatureAlert alert = myHeatDevice.getTemperatureAlert(i, forceNotification);
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
                manualTick = true;
            }

            MyHeatWeb::sendAlertNotification(message);
            MyHeatTelebot::sendAlertNotification(message);
        }

        if (manualTick)
        {
            myHeatDevice.manualTick();
        }
    }

    void checkSmokeAlerts(bool forceNotification)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        SmokeSensorAlert alert = myHeatDevice.MyHeatSmokeSensor::getSmokeSensorAlert(forceNotification);

        String message = "";

        if (alert == SSA_NONE) {
            return;
        }
        if (alert == SSA_BAD_CONNECTION)
        {
            message = "Димовий датчик не підключений";
        }
        else if (alert == SSA_OVER_THRESHOLD)
        {
            message = "Димовий датчик виявив дим";
        }

        MyHeatWeb::sendAlertNotification(message);
        MyHeatTelebot::sendAlertNotification(message);
    }

    void checkFunctionsAlerts(bool forceNotification) {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (byte i = 0; i < myHeatDevice.getCustomFunctionCount(); i++)
        {
            FunctionAlert alert = myHeatDevice.getFunctionAlert(i, forceNotification);

            String message = "";

            if (alert == FA_NONE)
            {
                continue;
            }
            else if (alert == FunctionAlert::FA_BAD_TEMPERATURE)
            {
                message = "Функція " + String(i) + " не може бути активною через проблеми з датчиками температури";
            }
            else if (alert == FunctionAlert::FA_BAD_TEMPERATURE_NUMBER)
            {
                message = "Функція " + String(i) + " не може бути активною через використання неіснуйочуго датчика температури";
            }
            else if (alert == FunctionAlert::FA_BAD_RELAY_NUMBER)
            {
                message = "Функція " + String(i) + " не може бути активною через використання неіснуйочуго реле";
            }

            MyHeatWeb::sendAlertNotification(message);
            MyHeatTelebot::sendAlertNotification(message);
        }
    }

}