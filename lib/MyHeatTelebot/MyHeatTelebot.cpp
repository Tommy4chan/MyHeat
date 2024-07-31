#include "MyHeatTelebot.h"

namespace MyHeatTelebot
{
    void begin(String token)
    {
        bot.setToken(token);
        bot.setPollMode(fb::Poll::Long, 20000);
        bot.attachUpdate(handleUpdate);
    }

    void tick()
    {
        bot.tick();
    }

    void handleUpdate(fb::Update &u)
    {
        if (u.isMessage())
        {
            if (u.message().text().startsWith('/'))
                handleCommand(u);
            else
                handleMessage(u);
        }
        else if (u.isQuery())
        {
            handleQuery(u);
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
                commands.addCommand("help", "Допомога");

                bot.setMyCommands(commands);

                msg.text = F("Привіт! \nЯ бот, для контролю системи опалення.");
            } break;
            case su::SH("/menu"):
            {
                msg.text = F("Головне меню");
                msg.setMenu(getMainReplyMenu());
            } break;
            case su::SH("/help"):
            {
                msg.text = F("Тут буде допомога");
            }
            break;
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
                msg.text = F("Температура: інфа");
            } break;
            case su::SH("Реле"):
            {
                msg.text = getRelayScreenText();
                msg.setInlineMenu(getRelayInlineMenu());
            } break;
            case su::SH("Функції"):
            {
                msg.text = F("Функції: інфа");
            } break;
            case su::SH("Допомога"):
            {
                msg.text = F("Допомога: інфа");
            } break;
        }

        bot.sendMessage(msg);
    }

    void handleQuery(fb::Update &u)
    {
        fb::QueryRead q = u.query();
        Serial.println(q.data());

        // // ответ
        bot.answerCallbackQuery(q.id());
        // bot.answerCallbackQuery(q.id(), q.data());
        // // bot.answerCallbackQuery(q.id(), q.data(), true);

        // if (q.data() == "change_msg")
        // {
        //     fb::TextEdit t;
        //     t.text = "New message + new menu";
        //     t.chatID = q.message().chat().id();
        //     t.messageID = q.message().id();
        //     fb::InlineMenu menu("kek 1;kek 2;kek 3", "1;2;3");
        //     t.setInlineMenu(menu);
        //     bot.editText(t);
        // }
        // else if (q.data() == "change_menu")
        // {
        //     fb::MenuEdit m;
        //     m.chatID = q.message().chat().id();
        //     m.messageID = q.message().id();
        //     fb::InlineMenu menu("pepe 1;pepe 2;pepe 3", "11;22;33");
        //     m.setInlineMenu(menu);
        //     bot.editMenu(m);
        // }
    }
}