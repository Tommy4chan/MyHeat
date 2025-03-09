#include "MyHeatWebTemperatures.h"

namespace MyHeatWeb
{
    void getDiscoveredTemperatureSensors(JsonObject response, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        byte count = myHeatDevice.discoverTemperatureSensor();
        uint8_t **addresses = myHeatDevice.getDiscoveredTemperatureSensorAddresses();
        for (int i = 0; i < count; i++)
        {
            response["discoveredTemperatureSensors"][i] = MyHeatUtils::getAddressToString(addresses[i]);
        }

        if (count == 0)
        {
            setWarningMessage(status, F("Датчики температури не знайдені"));
        }
        else
        {
            setSuccessMessage(status, F("Датчики температури знайдені"));
        }
    }

    void setTemperatureSensor(JsonObject payload, JsonObject status)
    {
        byte tempIndex = payload["tempIndex"];
        byte sensorAddressIndex = payload["sensorAddressIndex"];
        byte tempCount = MyHeatDevice::getInstance().getTemperatureCount();

        if(tempIndex >= tempCount || sensorAddressIndex >= tempCount)
        {
            setErrorMessage(status, F("Неправильний індекс датчика або адреси"));
            return;
        }

        MyHeatDevice::getInstance().setTemperatureSensorAddress(tempIndex, payload["sensorAddressIndex"]);
        setSuccessMessage(status, F("Датчик температури збережений"));
    }

    void deleteTemperatureSensor(JsonObject payload, JsonObject status)
    {
        MyHeatDevice::getInstance().deleteTemperatureSensorAddress(payload["tempIndex"]);

        setSuccessMessage(status, F("Датчик температури видалений"));
    }

    void getTemperatureSensorsSettings(JsonObject response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        response["temperaturePin"] = myHeatDevice.getTemperaturePin();
        response["temperatureCount"] = myHeatDevice.getTemperatureCount();
    }

    void setTemperatureSensorsSettings(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        myHeatDevice.setTemperaturePin(payload["temperaturePin"], false);
        myHeatDevice.setTemperatureCount(payload["temperatureCount"], false);
        myHeatDevice.MyHeatTemperatures::save();

        MyHeatHardwareIO::getInstance().reevaluateScreensCount();
        myHeatDevice.validateCustomFunctions();

        setSuccessMessage(status, F("Налаштування збережені"));
    }

    JsonDocument getTemperatureSensorsData()
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        JsonDocument temperaturesData;
        copyArray(myHeatDevice.getTemperatures(), myHeatDevice.getTemperatureCount(), temperaturesData[F("temperatures")]);
        return temperaturesData;
    }
}
