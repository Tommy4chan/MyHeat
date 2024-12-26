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

    String getTemperatureScreenText(float temperatures[])
    {
        String text = MyHeatUtils::getCurrentDateAndTime();

        for (byte i = 0; i < MyHeatDevice::getInstance().getTemperatureCount(); i++)
        {
            text += "T" + String(i) + ": " + (temperatures[i] == -127 ? "не визначено" : (String(temperatures[i])) + "°C") + "\n";
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