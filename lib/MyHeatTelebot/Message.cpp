#include "Message.h"

namespace MyHeatTelebot
{
    String getRelayScreenText(MyHeatRelay *relaysPtr)
    {
        String text = "";

        for (byte i = 0; i < MyHeatDevice::getInstance().getRelayCount(); i++)
        {
            text += "Реле " + String(i) + ": \n" + "Активне: " + MyHeatUtils::getConvertedActiveToText(relaysPtr[i].getIsActive()) + "\n\n";
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

    String getFunctionsListScreenText(MyHeatCustomFunction *customFunctionsPtr)
    {
        String text = "";

        for (byte i = 0; i < FUNCTION_COUNT; i++)
        {
            text += getConvertedFunctionToText(customFunctionsPtr[i], i);
        }

        return text;
    }

    String getFunctionScreenText(MyHeatCustomFunction customFunction, byte functionIndex)
    {
        return getConvertedFunctionToText(customFunction, functionIndex);
    }
}