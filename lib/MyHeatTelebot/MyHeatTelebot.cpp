#include "MyHeatTelebot.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

namespace MyHeatTelebot
{
    void begin(String token, MyHeatDevice *myHeatDevicePtr)
    {
        bot.setToken(token);
        bot.setPollMode(fb::Poll::Long, 20000);
        bot.attachUpdate(handleUpdate);
        usersDB.begin();

        MyHeatTelebot::myHeatDevicePtr = myHeatDevicePtr;
    }

    void tick()
    {
        bot.tick();
        usersDB.tick();
    }

    void handleUpdate(fb::Update &u)
    {
        if (u.isQuery())
        {
            handleQuery(u);
        }
        else if (u.isMessage())
        {
            if (!isUserRegistered(u.message().chat().id()))
            {
                Serial.println("User not registered");
                handleUserRegistration(u);
                return;
            }

            if (u.message().text().startsWith('/'))
                handleCommand(u);
            else
            {
                if (!isUserInputMode(u.message().chat().id()))
                {
                    handleMessage(u);
                }
                else
                {
                    handleUserInput(u);
                }
            }
        }

        // usersDB.dump(Serial);
    }

    void handleCommand(fb::Update &u)
    {
        Text chat_id = u.message().chat().id();

        fb::Message msg("", chat_id);

        switch (u.message().text().hash())
        {
        case su::SH("/start"):
        {
            fb::MyCommands commands;

            commands.addCommand("start", "Запустити бота");
            commands.addCommand("menu", "Головне меню");
            commands.addCommand("help", "Допомога");

            bot.setMyCommands(commands);

            msg.text = F("Привіт! \nЯ бот, для контролю системи опалення.");

            setUserScreen(chat_id, ScreenType::MAIN_SCREEN);
            break;
        }
        case su::SH("/menu"):
        {
            msg.text = F("Головне меню");
            msg.setMenu(getMainReplyMenu());

            setUserScreen(chat_id, ScreenType::MAIN_SCREEN);
            break;
        }
        case su::SH("/help"):
        {
            msg.text = F("Тут буде допомога");
            break;
        }
        }

        bot.sendMessage(msg);
    }

    void handleMessage(fb::Update &u)
    {
        Text chat_id = u.message().chat().id();
        String messageDecoded = u.message().text().decodeUnicode(); // temp solution, need to fix (maybe bug in FastBot2)
        Text message = messageDecoded;                              // temp solution, need to fix (maybe bug in FastBot2)
        fb::Message msg("", chat_id);

        switch (message.hash())
        {
        case su::SH("Температура"):
        {
            setUserScreen(chat_id, ScreenType::TEMP_SCREEN);
            msg.text = getTemperatureScreenText(myHeatDevicePtr->getTemperatures());
            msg.setInlineMenu(getTemperatureInlineMenu());
            break;
        }
        case su::SH("Реле"):
        {
            setUserScreen(msg.chatID, ScreenType::RELAYS_SCREEN);
            msg.text = getRelayScreenText(myHeatDevicePtr->getRelays());
            msg.setInlineMenu(getRelayInlineMenu(myHeatDevicePtr->getRelays()));
            break;
        }
        case su::SH("Функції"):
        {
            setUserScreen(chat_id, ScreenType::FUNCTIONS_LIST_SCREEN);
            msg.text = getFunctionsListScreenText(myHeatDevicePtr->getCustomFunctions());
            msg.setInlineMenu(getFunctionListInlineMenu());
            break;
        }
        case su::SH("Налаштування"):
        {
            msg.text = F("Налаштування: інфа");
            break;
        }
        case su::SH(STR(REGISTER_PHRASE)):
        {
            msg.text = F("Ви вже зареєстровані");
            break;
        }
        }

        bot.sendMessage(msg);
    }

    void handleQuery(fb::Update &u)
    {
        fb::QueryRead q = u.query();
        Serial.println(q.data());

        Text chat_id = q.message().chat().id();
        Text callback = getCallbackFromQuery(q.data());
        int value = getValueFromQuery(q.data());
        fb::TextEdit msg("", q.message().id(), chat_id);

        Serial.println(callback);
        Serial.println(value);

        switch (callback.hash())
        {
        case su::SH("discoverTemperatureSensor"):
        {
            setUserScreen(chat_id, ScreenType::TEMP_DISCOVER_SCREEN);
            byte addressesCount = myHeatDevicePtr->discoverTemperatureSensor();

            if (addressesCount) {
                msg.text = F("\nДатчики виявлено");
                msg.setInlineMenu(getDiscoveredTemperatureSensorsInlineMenu(addressesCount, myHeatDevicePtr->getDiscoveredTemperatureSensorAddresses()));
            } else {
                msg.text = F("Датчик не виявлено");
            }
            
            break;
        }
        case su::SH("deleteTemperatureSensor"):
        {
            setUserScreen(chat_id, ScreenType::TEMP_DELETE_SCREEN);

            setUserTempValue1(chat_id, value);
            msg.text = F("Виберіть датчик для видалення");
            msg.setInlineMenu(getTemperatureIndexesInlineMenu());
            break;
        }
        case su::SH("setTemperatureSensor"):
        {
            setUserTempValue1(chat_id, value);
            msg.text = F("Датчик вибрано");
            msg.setInlineMenu(getTemperatureIndexesInlineMenu());
            break;
        }
        case su::SH("setTemperatureIndex"):
        {
            User user = getUser(chat_id);
            if (user.screenType == ScreenType::TEMP_DISCOVER_SCREEN) {
                myHeatDevicePtr->setTemperatureSensorAddress(value, user.tempValue1);
                msg.text = F("Датчик встановлено \n\n");
            }
            else if (user.screenType == ScreenType::TEMP_DELETE_SCREEN) {
                myHeatDevicePtr->deleteTemperatureSensorAddress(value);
                msg.text = F("Датчик видалено \n\n");
            }

            setUserScreen(chat_id, ScreenType::TEMP_SCREEN);
            msg.text += getTemperatureScreenText(myHeatDevicePtr->getTemperatures());
            msg.setInlineMenu(getTemperatureInlineMenu());
            break;
        }
        case su::SH("relay"):
        {
            myHeatDevicePtr->changeRelayMode(value);
            msg.text = getRelayScreenText(myHeatDevicePtr->getRelays());
            msg.setInlineMenu(getRelayInlineMenu(myHeatDevicePtr->getRelays()));
            break;
        }
        case su::SH("function"):
        {
            setUserTempValue1(chat_id, value);
            setFunctionScreen(msg, myHeatDevicePtr->getCustomFunction(value), value);
            break;
        }
        case su::SH("functionChangeState"):
        {
            User user = getUser(chat_id);

            myHeatDevicePtr->setCustomFunctionIsEnabled(user.tempValue1);

            setFunctionScreen(msg, myHeatDevicePtr->getCustomFunction(user.tempValue1), user.tempValue1);
            break;
        }
        case su::SH("functionChangeSign"):
        {
            setUserScreen(chat_id, ScreenType::FUNCTION_CHANGE_SIGN_SCREEN);
            msg.text = F("Виберіть знак");
            msg.setInlineMenu(getFunctionChangeSignInlineMenu());
            break;
        }
        case su::SH("functionSetSign"):
        {
            User user = getUser(chat_id);

            if (user.screenType == ScreenType::FUNCTION_CHANGE_SIGN_SCREEN)
            {
                myHeatDevicePtr->setCustomFunctionSign(user.tempValue1, value);

                setFunctionScreen(msg, myHeatDevicePtr->getCustomFunction(user.tempValue1), user.tempValue1);
            }

            break;
        }
        case su::SH("functionChangeTemp"):
        {
            setUserScreen(chat_id, ScreenType::FUNCTION_CHANGE_TEMPERATURE_SCREEN);
            setUserTempValue2(chat_id, value);
            msg.text = F("Виберіть датчик температури");
            msg.setInlineMenu(getFunctionChangeTemperatureInlineMenu());
            break;
        }
        case su::SH("functionSetTemp"):
        {
            User user = getUser(chat_id);

            if (user.screenType == ScreenType::FUNCTION_CHANGE_TEMPERATURE_SCREEN)
            {
                myHeatDevicePtr->setCustomFunctionTemperatureIndex(user.tempValue1, user.tempValue2, value);

                setFunctionScreen(msg, myHeatDevicePtr->getCustomFunction(user.tempValue1), user.tempValue1);
            }

            break;
        }
        case su::SH("functionChangeRelay"):
        {
            setUserScreen(chat_id, ScreenType::FUNCTION_CHANGE_RELAY_SCREEN);
            msg.text = F("Виберіть реле");
            msg.setInlineMenu(getFunctionChangeRelayInlineMenu());
            break;
        }
        case su::SH("functionSetRelay"):
        {
            User user = getUser(chat_id);

            if (user.screenType == ScreenType::FUNCTION_CHANGE_RELAY_SCREEN)
            {
                myHeatDevicePtr->setCustomFunctionRelayIndex(user.tempValue1, value);

                setFunctionScreen(msg, myHeatDevicePtr->getCustomFunction(user.tempValue1), user.tempValue1);
            }

            break;
        }
        case su::SH("functionChangeDeltaTemp"):
        {
            setUserScreen(chat_id, ScreenType::FUNCTION_CHANGE_DELTA_TEMPERATURE_SCREEN);
            setUserTempValue2(chat_id, value);
            setUserInputMode(chat_id, true);
            msg.text = F("Введіть Δ значення: ");
            msg.setInlineMenu(getCancelFunctionInlineMenu());
            break;
        }
        case su::SH("cancelFunction"):
        {
            User user = getUser(chat_id);

            setFunctionScreen(msg, myHeatDevicePtr->getCustomFunction(user.tempValue1), user.tempValue1);
            setUserInputMode(chat_id, false);
            break;
        }
        case su::SH("closeFunction"):
        {
            setUserScreen(chat_id, ScreenType::FUNCTIONS_LIST_SCREEN);
            msg.text = getFunctionsListScreenText(myHeatDevicePtr->getCustomFunctions());
            msg.setInlineMenu(getFunctionListInlineMenu());
            break;
        }
        case su::SH("close"):
        {
            fb::Message closeMsg("", chat_id);
            setUserScreen(chat_id, ScreenType::MAIN_SCREEN);
            closeMsg.text = F("Головне меню");
            closeMsg.setMenu(getMainReplyMenu());

            bot.deleteMessage(chat_id, q.message().id());
            bot.sendMessage(closeMsg);
            return;
            break;
        }
        case su::SH("refreshFunction"):
        {
            User user = getUser(chat_id);
            if (user.screenType == ScreenType::FUNCTION_SCREEN)
            {
                setFunctionScreen(msg, myHeatDevicePtr->getCustomFunction(user.tempValue1), user.tempValue1);
            }
            break;
        }
        case su::SH("refreshFunctions"):
        {
            setUserScreen(chat_id, ScreenType::FUNCTIONS_LIST_SCREEN);
            msg.text = getFunctionsListScreenText(myHeatDevicePtr->getCustomFunctions());
            msg.setInlineMenu(getFunctionListInlineMenu());
            break;
        }
        case su::SH("refreshRelays"):
        {
            setUserScreen(chat_id, ScreenType::RELAYS_SCREEN);
            msg.text = getRelayScreenText(myHeatDevicePtr->getRelays());
            msg.setInlineMenu(getRelayInlineMenu(myHeatDevicePtr->getRelays()));
            break;
        }
        case su::SH("refreshTemperatures"):
        {
            msg.text += getTemperatureScreenText(myHeatDevicePtr->getTemperatures());
            msg.setInlineMenu(getTemperatureInlineMenu());
            break;
        }
        }

        bot.answerCallbackQuery(q.id());
        bot.editText(msg);
    }

    void handleUserRegistration(fb::Update &u)
    {
        Text chat_id = u.message().chat().id();
        fb::Message msg("", chat_id);

        if (u.message().text().hash() == su::SH(STR(REGISTER_PHRASE)))
        {
            registerUser(chat_id);
            msg.text = F("Вас зареєстровано, /start");
        }
        else
        {
            msg.text = F("Ви не зареєстровані, введіть фразу реєстрації");
        }

        bot.sendMessage(msg);
    }

    void handleUserInput(fb::Update &u)
    {
        Text chat_id = u.message().chat().id();
        fb::Message msg("", chat_id);

        Serial.println("Input mode");

        User user = getUser(u.message().chat().id());

        if (user.screenType == ScreenType::FUNCTION_CHANGE_DELTA_TEMPERATURE_SCREEN)
        {
            Serial.println("Change delta temp");

            if (!MyHeatUtils::isFloat(u.message().text()))
            {
                msg.text = F("Введіть коректне значення");
            }
            else
            {
                myHeatDevicePtr->setCustomFunctionDeltaValue(user.tempValue1, user.tempValue2, u.message().text().toFloat());
                setUserScreen(chat_id, ScreenType::FUNCTION_SCREEN);
                msg.text = getFunctionScreenText(myHeatDevicePtr->getCustomFunction(user.tempValue1), user.tempValue1);
                msg.setInlineMenu(getFunctionInlineMenu());
                setUserInputMode(chat_id, false);
            }
        }

        bot.sendMessage(msg);
    }
}