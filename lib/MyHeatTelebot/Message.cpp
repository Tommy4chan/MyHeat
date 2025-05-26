#include "Message.h"

namespace MyHeatTelebot
{
    String getRelayScreenText()
    {
        String text = "";
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (byte i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            text += "Реле " + String(i) + ": \n" + "Активне: " + MyHeatUtils::getConvertedActiveToText(myHeatDevice.getRelay(i).getIsActive()) + "\n\n";
        }

        return text;
    }

    String getTemperatureScreenText()
    {
        String text = MyHeatUtils::getCurrentDateAndTime();
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (byte i = 0; i < myHeatDevice.getTemperatureCount(); i++)
        {
            float temperature = myHeatDevice.getTemperature(i);
            text += "T" + String(i) + ": " + (temperature == -127 ? "Н/Д" : (String(temperature)) + "°C") + "\n";
        }

        return text;
    }

    String getFunctionsListScreenText()
    {
        String text = "";
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (byte i = 0; i < myHeatDevice.getCustomFunctionCount(); i++)
        {
            text += getConvertedFunctionToText(myHeatDevice.getCustomFunction(i), i);
        }

        return text;
    }

    String getFunctionScreenText(MyHeatCustomFunction customFunction, byte functionIndex)
    {
        return getConvertedFunctionToText(customFunction, functionIndex);
    }
}