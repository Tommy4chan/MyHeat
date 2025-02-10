#include "MyHeatWebTemperatures.h"

namespace MyHeatWeb
{
    void getDiscoveredTemperatureSensors(JsonObject response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        byte count = myHeatDevice.discoverTemperatureSensor();
        uint8_t **addresses = myHeatDevice.getDiscoveredTemperatureSensorAddresses();
        for (int i = 0; i < count; i++)
        {
            response["discoveredTemperatureSensors"][i] = MyHeatUtils::getAddressToString(addresses[i]);
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
    }

    void deleteTemperatureSensor(JsonObject payload)
    {
        MyHeatDevice::getInstance().deleteTemperatureSensorAddress(payload["tempIndex"]);
    }

    void getTemperatureSensorsSettings(JsonObject response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        response["temperaturePin"] = myHeatDevice.getTemperaturePin();
        response["temperatureCount"] = myHeatDevice.getTemperatureCount();
    }

    void setTemperatureSensorsSettings(JsonObject payload)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        myHeatDevice.setTemperaturePin(payload["temperaturePin"], false);
        myHeatDevice.setTemperatureCount(payload["temperatureCount"], false);
        myHeatDevice.MyHeatTemperatures::save();

        MyHeatHardwareIO::getInstance().reevaluateScreensCount();
        myHeatDevice.validateCustomFunctions();
    }

    JsonDocument getTemperatureSensorsData()
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        JsonDocument temperaturesData;
        copyArray(myHeatDevice.getTemperatures(), myHeatDevice.getTemperatureCount(), temperaturesData[F("temperatures")]);
        return temperaturesData;
    }
}
