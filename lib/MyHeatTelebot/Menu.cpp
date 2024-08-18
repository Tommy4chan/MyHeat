#include "Menu.h"

namespace MyHeatTelebot
{
    fb::Menu &getMainReplyMenu()
    {
        replyMenu.resize = 1;
        replyMenu.text = F("Температура ; Реле \n Функції ; Налаштування");

        return replyMenu;
    }

    fb::InlineMenu &getRelayInlineMenu()
    {
        inlineMenu = fb::InlineMenu();

        for (byte i = 0; i < RELAY_COUNT; i++)
        {
            inlineMenu.addButton("Реле " + String(i + 1) + ": " + MyHeatUtils::getConvertedStateToText(random(0, 3)), "relay_" + String(i)).newRow();
        }

        inlineMenu.addButton("Оновити", "refresh");

        return inlineMenu;
    }

    fb::InlineMenu &getTemperatureInlineMenu()
    {
        inlineMenu = fb::InlineMenu();

        inlineMenu.addButton("Оновити", "refresh");

        return inlineMenu;
    }

    fb::InlineMenu &getFunctionInlineMenu()
    {
        inlineMenu = fb::InlineMenu();

        for (byte i = 0; i < FUNCTION_COUNT; i++)
        {
            inlineMenu.addButton("Функція " + String(i + 1), "function_" + String(i));
            if (i % 2 != 0)
            {
                inlineMenu.newRow();
            }
        }

        if (FUNCTION_COUNT % 2 != 0)
        {
            inlineMenu.newRow();
        }

        inlineMenu.addButton("Оновити", "refresh");

        return inlineMenu;
    }
}