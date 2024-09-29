#include "Menu.h"

namespace MyHeatTelebot
{
    fb::Menu &getMainReplyMenu()
    {
        replyMenu.resize = 1;
        replyMenu.text = F("Температура ; Реле \n Функції ; Налаштування");

        return replyMenu;
    }

    fb::InlineMenu &getRelayInlineMenu(MyHeatRelay *relaysPtr)
    {
        inlineMenu = fb::InlineMenu();

        for (byte i = 0; i < RELAY_COUNT; i++)
        {
            inlineMenu.addButton("Реле " + String(i + 1) + ": " + MyHeatUtils::getConvertedStateToText(relaysPtr[i].getMode()), "relay_" + String(i)).newRow();
        }

        inlineMenu.addButton("Оновити", "refreshRelays");

        return inlineMenu;
    }

    fb::InlineMenu &getTemperatureInlineMenu()
    {
        inlineMenu = fb::InlineMenu(
            F("Виявити датчик \n Видалити датчик \n Оновити"),
            F("discoverTemperatureSensor;deleteTemperatureSensor;refreshTemperatures"));


        return inlineMenu;
    }

    fb::InlineMenu &getDiscoveredTemperatureSensorsInlineMenu(byte count, uint8_t addresses[][8]) 
    {
        inlineMenu = fb::InlineMenu();

        for (byte i = 0; i < count; i++)
        {
            inlineMenu.addButton(MyHeatUtils::getAddressToString(addresses[i]), "setTemperatureSensor_" + String(i));
            if (i % 2 != 0 && i != count - 1)
            {
                inlineMenu.newRow();
            }
        }

        return inlineMenu;
    }

    fb::InlineMenu &getTemperatureIndexesInlineMenu() 
    {
        inlineMenu = fb::InlineMenu();

        for (byte i = 0; i < TEMPERATURE_COUNT; i++)
        {
            inlineMenu.addButton("T" + String(i), "setTemperatureIndex_" + String(i));
            if (i % 2 != 0 && i != TEMPERATURE_COUNT - 1)
            {
                inlineMenu.newRow();
            }
        }

        return inlineMenu;
    }

    fb::InlineMenu &getFunctionListInlineMenu()
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

        inlineMenu.addButton("Оновити", "refreshFunctions");

        return inlineMenu;
    }

    fb::InlineMenu &getFunctionInlineMenu()
    {
        inlineMenu = fb::InlineMenu(
            F("Увімкнути/Вимкнути \n Змінити знак \n Змінити T[0] \n Змінити ΔT[0] \n Змінити T[1] \n Змінити ΔT[1] \n Змінити реле \n Оновити \n Назад"),
            F("functionChangeState;functionChangeSign;functionChangeTemp_0;functionChangeDeltaTemp_0;functionChangeTemp_1;functionChangeDeltaTemp_1;functionChangeRelay;refreshFunction;closeFunction"));

        return inlineMenu;
    }

    fb::InlineMenu &getFunctionChangeSignInlineMenu()
    {
        inlineMenu = fb::InlineMenu(
            F("< ; = ; > \n Скасувати"),
            F("functionSetSign_0;functionSetSign_1;functionSetSign_2;cancelFunction"));

        return inlineMenu;
    }

    fb::InlineMenu &getFunctionChangeTemperatureInlineMenu()
    {
        inlineMenu = fb::InlineMenu();

        for (byte i = 0; i < TEMPERATURE_COUNT; i++)
        {
            inlineMenu.addButton("T" + String(i), "functionSetTemp_" + String(i));
            if (i % 2 != 0)
            {
                inlineMenu.newRow();
            }
        }

        inlineMenu.addButton(F("TN"), "functionSetTemp_" + String(TEMPERATURE_COUNT));

        return inlineMenu;
    }

    fb::InlineMenu &getFunctionChangeRelayInlineMenu()
    {
        inlineMenu = fb::InlineMenu();

        for (byte i = 0; i < RELAY_COUNT; i++)
        {
            inlineMenu.addButton("Реле " + String(i), "functionSetRelay_" + String(i));
            if (i % 2 != 0 && i != RELAY_COUNT - 1)
            {
                inlineMenu.newRow();
            }
        }

        return inlineMenu;
    }

    fb::InlineMenu &getCancelFunctionInlineMenu()
    {
        inlineMenu = fb::InlineMenu(F("Скасувати"), F("cancelFunction"));

        return inlineMenu;
    }

}