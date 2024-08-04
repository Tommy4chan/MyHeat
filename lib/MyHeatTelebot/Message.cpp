#include "Message.h"

namespace MyHeatTelebot
{
    String getRelayScreenText()
    {
        String text = "";

        for (byte i = 0; i < RELAY_COUNT; i++)
        {
            text += "Реле " + String(i + 1) + ": \n" + "Статус: " + MyHeatUtils::getConvertedStateToText(random(0, 3)) + "\n\n";
        }

        return text;
    }

    String getTemperatureScreenText()
    {
        String text = MyHeatUtils::getCurrentDateAndTime();

        for (byte i = 0; i < TEMPERATURE_COUNT; i++)
        {
            text += "T" + String(i) + ": " + String((float)random(2000, 8000) / 100) + "°C" "\n";
        }

        return text;
    }
}