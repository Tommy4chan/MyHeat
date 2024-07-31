#include "Menus.h"

namespace MyHeatTelebot
{
    fb::Menu &getMainReplyMenu()
    {
        replyMenu.text = F("Температура ; Реле \n Функції ; Допомога");

        return replyMenu;
    }

    fb::InlineMenu &getRelayInlineMenu()
    {
        inlineMenu = fb::InlineMenu();

        for (byte i = 0; i < RELAY_COUNT; i++)
        {
            inlineMenu.addButton("Реле " + String(i + 1), "relay_" + String(i));
        }

        return inlineMenu;
    }
}