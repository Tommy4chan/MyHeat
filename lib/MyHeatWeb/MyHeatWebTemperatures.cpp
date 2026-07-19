#include "MyHeatWebTemperatures.h"

namespace MyHeatWeb
{
    void getDiscoveredTemperatureSensors(JsonObject response, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        byte count = myHeatDevice.temperatures.discoverTemperatureSensor();
        std::vector<std::array<uint8_t, 8>>& addresses = myHeatDevice.temperatures.getDiscoveredTemperatureSensorAddresses();
        for (int i = 0; i < count; i++)
        {
            response["discoveredTemperatureSensors"][i] = MyHeatUtils::getAddressToString(addresses[i].data());
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
        byte tempCount = MyHeatDevice::getInstance().temperatures.getTemperatureCount();

        if(tempIndex >= tempCount || sensorAddressIndex >= tempCount)
        {
            setErrorMessage(status, F("Неправильний індекс датчика або адреси"));
            return;
        }

        MyHeatDevice::getInstance().temperatures.setTemperatureSensorAddress(tempIndex, payload["sensorAddressIndex"]);
        setSuccessMessage(status, F("Датчик температури збережений"));
    }

    void deleteTemperatureSensor(JsonObject payload, JsonObject status)
    {
        MyHeatDevice::getInstance().temperatures.deleteTemperatureSensorAddress(payload["tempIndex"]);

        setSuccessMessage(status, F("Датчик температури видалений"));
    }

    void getTemperatureSensorsSettings(JsonObject response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        response["temperaturePin"] = myHeatDevice.temperatures.getTemperaturePin();
        response["temperatureCount"] = myHeatDevice.temperatures.getTemperatureCount();
        response["minTemperature"] = myHeatDevice.temperatures.getMinTemperature();
        response["maxTemperature"] = myHeatDevice.temperatures.getMaxTemperature();
    }

    void setTemperatureSensorsSettings(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        myHeatDevice.temperatures.setTemperatureSettings(payload["temperaturePin"], payload["temperatureCount"], payload["minTemperature"], payload["maxTemperature"]);

        MyHeatHardwareIO::getInstance().reevaluateScreensCount();
        myHeatDevice.validateCustomFunctions();

        setSuccessMessage(status, F("Налаштування збережені"));
    }

    JsonDocument getTemperatureSensorsData()
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        JsonDocument temperaturesData;
        copyArray(myHeatDevice.temperatures.getTemperatures().data(), myHeatDevice.temperatures.getTemperatureCount(), temperaturesData[F("temperatures")]);
        return temperaturesData;
    }
}
