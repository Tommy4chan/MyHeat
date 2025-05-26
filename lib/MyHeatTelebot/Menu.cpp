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

        for (byte i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            inlineMenu.addButton("Реле " + String(i) + ": " + MyHeatUtils::getConvertedStateToText(myHeatDevice.getRelay(i).getMode()), "relay_" + String(i)).newRow();
        }

        inlineMenu.addButton("Оновити", "refreshRelays");

        return inlineMenu;
    }

    fb::InlineMenu &getTemperatureInlineMenu()
    {
        // inlineMenu = fb::InlineMenu(
        //     F("Виявити датчик \n Видалити датчик \n Оновити"),
        //     F("discoverTemperatureSensor;deleteTemperatureSensor;refreshTemperatures"));

        inlineMenu = fb::InlineMenu(
            F("Оновити"),
            F("refreshTemperatures")
        );

        return inlineMenu;
    }

    fb::InlineMenu &getDiscoveredTemperatureSensorsInlineMenu(byte count, uint8_t **addresses) 
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

        byte temperatureCount = MyHeatDevice::getInstance().getTemperatureCount();

        for (byte i = 0; i < temperatureCount; i++)
        {
            inlineMenu.addButton("T" + String(i), "setTemperatureIndex_" + String(i));
            if (i % 2 != 0 && i != temperatureCount - 1)
            {
                inlineMenu.newRow();
            }
        }

        return inlineMenu;
    }

    fb::InlineMenu &getFunctionListInlineMenu()
    {
        inlineMenu = fb::InlineMenu();
        byte functionCount = MyHeatDevice::getInstance().getCustomFunctionCount();

        for (byte i = 0; i < functionCount; i++)
        {
            inlineMenu.addButton("Функція " + String(i + 1), "function_" + String(i));
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

        for (byte i = 0; i < MyHeatDevice::getInstance().getTemperatureCount(); i++)
        {
            inlineMenu.addButton("T" + String(i), "functionSetTemp_" + String(i));
            if (i % 2 != 0)
            {
                inlineMenu.newRow();
            }
        }

        inlineMenu.addButton(F("TN"), "functionSetTemp_" + String(TN_INDEX));

        return inlineMenu;
    }

    fb::InlineMenu &getFunctionChangeRelayInlineMenu()
    {
        inlineMenu = fb::InlineMenu();

        byte relayCount = MyHeatDevice::getInstance().getRelayCount(); 

        for (byte i = 0; i < relayCount; i++)
        {
            inlineMenu.addButton("Реле " + String(i), "functionSetRelay_" + String(i));
            if (i % 2 != 0 && i != relayCount - 1)
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