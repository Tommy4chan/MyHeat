#include "Message.h"

namespace MyHeatTelebot
{
    String getRelayScreenText()
    {
        String text = "";

        for (byte i = 0; i < RELAY_COUNT; i++)
        {
            text += "Реле " + String(i + 1) + ": \n" + "Активне: " + MyHeatUtils::getConvertedActiveToText(random(0, 2)) + "\n\n";
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

    String getFunctionScreenText()
    {
        String text = "";

        for (byte i = 0; i < FUNCTION_COUNT; i++)
        {
            text += getConvertedFunctionToText(i);
        }

        return text;
    }
}