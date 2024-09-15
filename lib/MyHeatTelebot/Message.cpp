#include "Message.h"

namespace MyHeatTelebot
{
    String getRelayScreenText(MyHeatRelay *relaysPtr)
    {
        String text = "";

        for (byte i = 0; i < RELAY_COUNT; i++)
        {
            text += "Реле " + String(i + 1) + ": \n" + "Активне: " + MyHeatUtils::getConvertedActiveToText(relaysPtr[i].getIsActive()) + "\n\n";
        }

        return text;
    }

    String getTemperatureScreenText()
    {
        String text = MyHeatUtils::getCurrentDateAndTime();

        for (byte i = 0; i < TEMPERATURE_COUNT; i++)
        {
            text += "T" + String(i) + ": " + String((float)random(2000, 8000) / 100) + "°C\n";
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