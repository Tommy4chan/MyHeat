#include "Menu.h"

namespace MyHeatTelebot
{
    fb::Menu &getMainReplyMenu()
    {
        replyMenu.text = F("Температура ; Реле \n Функції ; Налаштування");

        return replyMenu;
    }

    fb::InlineMenu &getRelayInlineMenu()
    {
        inlineMenu = fb::InlineMenu();

        for (byte i = 0; i < RELAY_COUNT; i++)
        {
            inlineMenu.addButton("Реле " + String(i + 1) + ": " + MyHeatUtils::getConvertedStateToText(random(0, 2)), "relay_" + String(i)).newRow();
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
}