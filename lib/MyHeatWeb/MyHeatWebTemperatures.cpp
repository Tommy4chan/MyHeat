#include "MyHeatWebTemperatures.h"

namespace MyHeatWeb
{
    void getDiscoveredTemperatureSensors(JsonDocument &response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        byte count = myHeatDevice.discoverTemperatureSensor();
        uint8_t **addresses = myHeatDevice.getDiscoveredTemperatureSensorAddresses();
        for (int i = 0; i < count; i++)
        {
            response["payload"]["discoveredTemperatureSensors"][i] = MyHeatUtils::getAddressToString(addresses[i]);
        }
    }

    void setTemperatureSensor(JsonObject payload)
    {
        MyHeatDevice::getInstance().setTemperatureSensorAddress(payload["tempIndex"], payload["sensorAddressIndex"]);
    }

    void deleteTemperatureSensor(JsonObject payload)
    {
        MyHeatDevice::getInstance().deleteTemperatureSensorAddress(payload["tempIndex"]);
    }

    void getTemperatureSensorsSettings(JsonDocument &response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        response["payload"]["temperaturePin"] = myHeatDevice.getTemperaturePin();
        response["payload"]["temperatureCount"] = myHeatDevice.getTemperatureCount();
    }

    void setTemperatureSensorsSettings(JsonObject payload)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        myHeatDevice.setTemperaturePin(payload["temperaturePin"]);
        myHeatDevice.setTemperatureCount(payload["temperatureCount"]);
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
