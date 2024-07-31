#include "MessageTexts.h"

namespace MyHeatTelebot
{
    String getRelayScreenText()
    {
        String text = "";

        for (byte i = 0; i < RELAY_COUNT; i++)
        {
            text += "Реле " + String(i + 1) + ": \n" + "Стан: Авто" + "\nСтатус: Увімкнено" + "\n\n";
        }

        return text;
    }
}