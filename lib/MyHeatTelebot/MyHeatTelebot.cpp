#include "MyHeatTelebot.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

namespace MyHeatTelebot
{
    void begin(String token)
    {
        bot.setToken(token);
        bot.setPollMode(fb::Poll::Long, 20000);
        bot.attachUpdate(handleUpdate);
        usersDB.begin();
    }

    void tick()
    {
        bot.tick();
        usersDB.tick();
    }

    void handleUpdate(fb::Update &u)
    {
        if (u.isMessage())
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
                handleMessage(u);
        }
        else if (u.isQuery())
        {
            handleQuery(u);
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
                msg.text = getTemperatureScreenText();
                msg.setInlineMenu(getTemperatureInlineMenu());
                break;
            }
            case su::SH("Реле"):
            {
                msg.text = getRelayScreenText();
                msg.setInlineMenu(getRelayInlineMenu());
                break;
            }
            case su::SH("Функції"):
            {
                msg.text = getFunctionsListScreenText();
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
            case su::SH("function"):
            {
                setUserScreen(chat_id, ScreenType::FUNCTION_SCREEN);
                setUserTempValue1(chat_id, value);
                msg.text = getFunctionScreenText(value);
                msg.setInlineMenu(getFunctionInlineMenu());
                break;
            }
            case su::SH("functionChangeState"):
            {
                msg.text = getFunctionScreenText(value);
                //here change function state
                msg.setInlineMenu(getFunctionInlineMenu());
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
                    //here change function sign
                    Serial.println("Change sign to");
                    Serial.println(value);
                    
                    setUserScreen(chat_id, ScreenType::FUNCTION_SCREEN);
                    msg.text = getFunctionScreenText(user.tempValue1);
                    msg.setInlineMenu(getFunctionInlineMenu());
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
                    //here change function temp sensor
                    Serial.println("Change temp to");
                    Serial.println(user.tempValue2);
                    Serial.println(value);
                    
                    setUserScreen(chat_id, ScreenType::FUNCTION_SCREEN);
                    msg.text = getFunctionScreenText(user.tempValue1);
                    msg.setInlineMenu(getFunctionInlineMenu());
                }
                
                break;
            }
            case su::SH("functionChangeRelay"):
            {
                setUserScreen(chat_id, ScreenType::FUNCTION_CHANGE_RELAY_SCREEN);
                setUserTempValue2(chat_id, value);
                msg.text = F("Виберіть реле");
                msg.setInlineMenu(getFunctionChangeRelayInlineMenu());
                break;
            }
            case su::SH("functionSetRelay"):
            {
                User user = getUser(chat_id);
                
                if (user.screenType == ScreenType::FUNCTION_CHANGE_RELAY_SCREEN)
                {
                    //here change function relay
                    Serial.println("Change relay to");
                    Serial.println(value);
                    
                    setUserScreen(chat_id, ScreenType::FUNCTION_SCREEN);
                    msg.text = getFunctionScreenText(user.tempValue1);
                    msg.setInlineMenu(getFunctionInlineMenu());
                }
                
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
}