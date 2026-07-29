#include "Menu.h"

namespace MyHeatTelebot
{
    fb::Menu &getMainReplyMenu()
    {
        replyMenu.resize = 1;
        replyMenu.text = F("Температура ; Реле \n Функції ; Інформація");

        return replyMenu;
    }

    fb::InlineMenu &getRelayInlineMenu()
    {
        inlineMenu = fb::InlineMenu();
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (byte i = 0; i < myHeatDevice.relays.getRelayCount(); i++)
        {
            inlineMenu.addButton("Реле " + String(i) + ": " + MyHeatUtils::getConvertedStateToText(static_cast<byte>(myHeatDevice.relays.getRelay(i).getMode())), "relay_" + String(i)).newRow();
        }

        inlineMenu.addButton("Оновити", "refreshRelays");

        return inlineMenu;
    }

    fb::InlineMenu &getTemperatureInlineMenu()
    {
        inlineMenu = fb::InlineMenu(
            F("Оновити"),
            F("refreshTemperatures")
        );

        return inlineMenu;
    }

    fb::InlineMenu &getFunctionListInlineMenu()
    {
        inlineMenu = fb::InlineMenu();
        byte functionCount = MyHeatDevice::getInstance().customFunctions.getCustomFunctionCount();

        for (byte i = 0; i < functionCount; i++)
        {
            inlineMenu.addButton("Функція " + String(i), "function_" + String(i));
            if (i % 2 != 0)
            {
                inlineMenu.newRow();
            }
        }

        if (functionCount % 2 != 0)
        {
            inlineMenu.newRow();
        }

        inlineMenu.addButton("Оновити", "refreshFunctions");

        return inlineMenu;
    }

    fb::InlineMenu &getFunctionInlineMenu(byte functionIndex)
    {
        inlineMenu = fb::InlineMenu();
        String idx = String(functionIndex);
        inlineMenu.addButton(F("Увімкнути/Вимкнути"), "functionChangeState_" + idx);
        inlineMenu.newRow();
        inlineMenu.addButton(F("Оновити"), "refreshFunction_" + idx);
        inlineMenu.newRow();
        inlineMenu.addButton(F("Назад"), "closeFunction");
        return inlineMenu;
    }

}