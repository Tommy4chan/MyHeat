#include "MyHeatTelebot.h"
#include <MyHeatAlerts.h>

namespace MyHeatTelebot
{
    static std::vector<String> pendingAlerts;
    static const byte MAX_PENDING_ALERTS = 10;

    void begin()
    {
        botData = new MyHeatSave("/telebot.json", &botSave);
        botData->read();

        bot.setToken(botSave.token);
        bot.setPollMode(fb::Poll::Long, 30000);
        bot.attachUpdate(handleUpdate);
        loadUsers();

        MyHeatAlerts::registerHandler(MyHeatTelebot::sendAlertNotification);
    }

    void tick()
    {
        if (botSave.isEnabled && botSave.token.length() > 0)
        {
            if (MyHeatWifi::getInstance().isTimeSynced())
            {
                if (!pendingAlerts.empty() && MyHeatWifi::getInstance().isConnected())
                {
                    for (const auto& message : pendingAlerts)
                    {
                        for (byte i = 0; i < getUsersCount(); i++)
                        {
                            fb::Message msg(message, getChatIdByIndex(i));
                            bot.sendMessage(msg);
                        }
                    }
                    pendingAlerts.clear();
                }

                // TODO: Add a preloader/connecting screen on OLED here because bot.tick() blocks loop for ~7s during the initial DNS/SSL handshake
                bot.tick();
            }
        }
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
                handleUserRegistration(u);
                return;
            }

            if (u.message().text().startsWith('/'))
                handleCommand(u);
            else
            {
                handleMessage(u);
            }
        }
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

            bot.setMyCommands(commands);

            msg.text = F("Привіт! \nЯ бот, для контролю системи опалення.");

            break;
        }
        case su::SH("/menu"):
        {
            msg.text = F("Головне меню");
            msg.setMenu(getMainReplyMenu());

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
        case su::SH("Інформація"):
        {
            MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

            // HTML mode so passwords can be hidden as spoilers, user values are escaped
            msg.mode = fb::Message::Mode::HTML;
            msg.text = "Датчик диму: \n"
                       "Значення: " +
                       (myHeatDevice.smokeSensor.getValue() == -1 ? "Ініціалізація" : String(myHeatDevice.smokeSensor.getValue())) + "\n" +
                       "Увімкнений: " + MyHeatUtils::getConvertedActiveToText(myHeatDevice.smokeSensor.getIsEnabled()) + "\n" +
                       "Поріг спрацювання: " + String(myHeatDevice.smokeSensor.getThreshold()) + " \n\n" +
                       "Веб-інтерфейс: http://" + escapeHtml(myHeatWifi.getMDNS()) + ".local\n\n" +
                       "Мережа: \n" +
                       "Ім'я: " + escapeHtml(myHeatWifi.getWifiSSID()) + "\n" +
                       "Пароль: <tg-spoiler>" + escapeHtml(myHeatWifi.getWifiPassword()) + "</tg-spoiler>\n\n" +
                       "Точка доступу: \n" +
                       "Ім'я: " + escapeHtml(myHeatWifi.getAPSSID()) + "\n" +
                       "Пароль: <tg-spoiler>" + escapeHtml(myHeatWifi.getAPPassword()) + "</tg-spoiler>\n" +
                       "Режим резервної точки доступу: " + MyHeatUtils::getConvertedStateToText(myHeatWifi.getIsFallbackAPEnabled());
            break;
        }
        }

        bot.sendMessage(msg);
    }

    void handleQuery(fb::Update &u)
    {
        fb::QueryRead q = u.query();

        Text chat_id = q.message().chat().id();
        Text callback = getCallbackFromQuery(q.data());
        int value = getValueFromQuery(q.data());
        fb::TextEdit msg("", q.message().id(), chat_id);

        switch (callback.hash())
        {
        case su::SH("relay"):
        {
            if (value >= myHeatDevice.relays.getRelayCount())
            {
                msg.text = getRelayScreenText();
                msg.setInlineMenu(getRelayInlineMenu());
                break;
            }
            myHeatDevice.relays.changeRelayMode(value);
            myHeatDevice.manualTick();

            msg.text = getRelayScreenText();
            msg.setInlineMenu(getRelayInlineMenu());
            break;
        }
        case su::SH("function"):
        {
            if (value >= myHeatDevice.customFunctions.getCustomFunctionCount())
            {
                msg.text = F("Функцію не знайдено. Оновіть список.");
                msg.setInlineMenu(getFunctionListInlineMenu());
                break;
            }
            setFunctionScreen(msg, myHeatDevice.customFunctions.getCustomFunction(value), value);
            break;
        }
        case su::SH("functionChangeState"):
        {
            if (value >= myHeatDevice.customFunctions.getCustomFunctionCount())
            {
                msg.text = F("Функцію не знайдено. Оновіть список.");
                msg.setInlineMenu(getFunctionListInlineMenu());
                break;
            }
            bool currentState = myHeatDevice.customFunctions.getCustomFunction(value).getIsEnabled();
            myHeatDevice.customFunctions.setCustomFunctionIsEnabled(value, !currentState);
            myHeatDevice.manualTick();

            setFunctionScreen(msg, myHeatDevice.customFunctions.getCustomFunction(value), value);
            break;
        }
        case su::SH("closeFunction"):
        {
            msg.text = getFunctionsListScreenText();
            msg.setInlineMenu(getFunctionListInlineMenu());
            break;
        }
        case su::SH("close"):
        {
            fb::Message closeMsg("", chat_id);
            closeMsg.text = F("Головне меню");
            closeMsg.setMenu(getMainReplyMenu());

            bot.deleteMessage(chat_id, q.message().id());
            bot.sendMessage(closeMsg);
            return;
        }
        case su::SH("refreshFunction"):
        {
            if (value >= myHeatDevice.customFunctions.getCustomFunctionCount())
            {
                msg.text = F("Функцію не знайдено. Оновіть список.");
                msg.setInlineMenu(getFunctionListInlineMenu());
                break;
            }
            setFunctionScreen(msg, myHeatDevice.customFunctions.getCustomFunction(value), value);
            break;
        }
        case su::SH("refreshFunctions"):
        {
            msg.text = getFunctionsListScreenText();
            msg.setInlineMenu(getFunctionListInlineMenu());
            break;
        }
        case su::SH("refreshRelays"):
        {
            msg.text = getRelayScreenText();
            msg.setInlineMenu(getRelayInlineMenu());
            break;
        }
        case su::SH("refreshTemperatures"):
        {
            msg.text = getTemperatureScreenText();
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

        if (u.message().text().hash() == su::SH(botSave.registerPhrase.c_str()))
        {
            registerUser(chat_id);
            msg.text = F("Вас зареєстровано, /menu");
        }
        else
        {
            msg.text = F("Ви не зареєстровані, введіть фразу реєстрації");
        }

        bot.sendMessage(msg);
    }

    void setToken(const String& token)
    {
        botSave.token = token;
        bot.setToken(token);
        save();
    }

    void setRegisterPhrase(const String& registerPhrase)
    {
        botSave.registerPhrase = registerPhrase;
        save();
    }

    void setIsEnabled(bool isEnabled)
    {
        botSave.isEnabled = isEnabled;
        save();
    }

    String getToken()
    {
        return botSave.token;
    }

    String getRegisterPhrase()
    {
        return botSave.registerPhrase;
    }

    bool getIsEnabled()
    {
        return botSave.isEnabled;
    }

    bool getIsAlertNotificationsEnabled()
    {
        return botSave.isAlertNotificationsEnabled;
    }

    void setSettings(const String& token, const String& registerPhrase, bool isEnabled, bool isAlertNotificationsEnabled)
    {
        botSave.token = token;
        bot.setToken(token);
        botSave.registerPhrase = registerPhrase;
        botSave.isEnabled = isEnabled;
        botSave.isAlertNotificationsEnabled = isAlertNotificationsEnabled;
        save();
    }

    void manualDeserialize(const JsonDocument& data)
    {
        botSave.manualDeserialize(data);
        save();
        bot.setToken(botSave.token);
    }

    void sendAlertNotification(const String& message)
    {
        if (!botSave.isEnabled || !botSave.isAlertNotificationsEnabled)
            return;

        if (!MyHeatWifi::getInstance().isConnected())
        {
            for (const auto& pending : pendingAlerts)
            {
                if (pending == message) return;
            }
            if (pendingAlerts.size() < MAX_PENDING_ALERTS)
            {
                pendingAlerts.push_back(message);
            }
            return;
        }

        for (byte i = 0; i < getUsersCount(); i++)
        {
            fb::Message msg(message, getChatIdByIndex(i));
            bot.sendMessage(msg);
        }
    }

    void save()
    {
        botData->save();
    }
}