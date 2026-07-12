#include "Message.h"

namespace MyHeatTelebot
{
    String getRelayScreenText()
    {
        String text = "";
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        text.reserve(myHeatDevice.relays.getRelayCount() * 40);

        for (byte i = 0; i < myHeatDevice.relays.getRelayCount(); i++)
        {
            text += "Реле " + String(i) + ": \n" + "Активне: " + MyHeatUtils::getConvertedActiveToText(myHeatDevice.relays.getRelay(i).getIsActive()) + "\n\n";
        }

        return text;
    }

    String getTemperatureScreenText()
    {
        String text = MyHeatUtils::getCurrentDateAndTime();
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        text.reserve(text.length() + myHeatDevice.temperatures.getTemperatureCount() * 20);

        for (byte i = 0; i < myHeatDevice.temperatures.getTemperatureCount(); i++)
        {
            float temperature = myHeatDevice.temperatures.getTemperature(i);
            text += "T" + String(i) + ": " + (temperature == -127 ? "Н/Д" : (String(temperature)) + "°C") + "\n";
        }

        return text;
    }

    String getFunctionsListScreenText()
    {
        String text = "";
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        text.reserve(myHeatDevice.customFunctions.getCustomFunctionCount() * 80);

        for (byte i = 0; i < myHeatDevice.customFunctions.getCustomFunctionCount(); i++)
        {
            text += getConvertedFunctionToText(myHeatDevice.customFunctions.getCustomFunction(i), i);
        }

        return text;
    }

    String getFunctionScreenText(const MyHeatCustomFunction& customFunction, byte functionIndex)
    {
        return getConvertedFunctionToText(customFunction, functionIndex);
    }
}