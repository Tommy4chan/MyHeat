#include "MyHeatTelebot.h"

namespace MyHeatTelebot
{
    FastBot2 bot;

    void begin(String token)
    {
        bot.setToken(token);
        bot.setPollMode(fb::Poll::Async, 4000);
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

        switch (u.message().text().hash())
        {
        case su::SH("/start"):
        {
            fb::MyCommands commands;

            commands.addCommand("start", "Запустити бота");
            commands.addCommand("menu", "Головне меню");
            commands.addCommand("help", "Допомога");
            commands.addCommand("settings", "Налаштування");

            bot.setMyCommands(commands);

            fb::Message msg;
            msg.text = F(
                "Привіт! \nЯ бот, який допоможе тобі зробити купівлю в інтернет-магазині.");
            msg.chatID = chat_id;
            bot.sendMessage(msg);
        }
        break;
        case su::SH("/menu"):
        {
            bot.sendMessage(fb::Message("Some text", chat_id));
        }
        break;

        case su::SH("/send_menu"):
        {
            fb::Message msg("Send menu", chat_id);

            // меню, вариант 1
            // fb::Menu menu;
            // menu.text = "kek 1 ; kek 2 ; kek 3 \n kek 4 ; kek 5";
            // menu.resize = 1;
            // menu.placeholder = "Fancy placeholder";
            // msg.setMenu(menu);

            // меню, вариант 2
            fb::Menu menu;
            menu.addButton("kek 1").addButton("kek 2").newRow();
            menu.addButton("kek 3");
            msg.setMenu(menu);

            bot.sendMessage(msg);
        }
        break;

        case su::SH("/send_inline_menu"):
        {
            fb::Message msg("Send inline menu", chat_id);

            // inline menu, вариант 1
            // fb::InlineMenu menu("kek 1 ; kek 2 ; kek 3 \n kek 4 ; kek 5", "test;pest;lol;https://www.google.ru/;https://www.yandex.ru/");
            // msg.setInlineMenu(menu);

            // inline menu, вариант 2
            fb::InlineMenu menu;
            menu.addButton("BUTTON 1"); // data == text
            menu.addButton("BUTTON 2", "data_2");
            menu.newRow();
            menu.addButton("BUTTON 3", "https://www.google.ru/");
            menu.newRow();
            menu.addButton("Change message", "change_msg");
            menu.addButton("Change menu", "change_menu");
            msg.setInlineMenu(menu);

            bot.sendMessage(msg);
        }
        break;
        }
    }

    void handleMessage(fb::Update &u)
    {
        Text chat_id = u.message().chat().id();

        bot.sendMessage(fb::Message(u.message().text(), chat_id));
    }

    void handleQuery(fb::Update &u)
    {
        fb::QueryRead q = u.query();
        Serial.println(q.data());

        // ответ
        // bot.answerCallbackQuery(q.id());
        bot.answerCallbackQuery(q.id(), q.data());
        // bot.answerCallbackQuery(q.id(), q.data(), true);

        if (q.data() == "change_msg")
        {
            fb::TextEdit t;
            t.text = "New message + new menu";
            t.chatID = q.message().chat().id();
            t.messageID = q.message().id();
            fb::InlineMenu menu("kek 1;kek 2;kek 3", "1;2;3");
            t.setInlineMenu(menu);
            bot.editText(t);
        }
        else if (q.data() == "change_menu")
        {
            fb::MenuEdit m;
            m.chatID = q.message().chat().id();
            m.messageID = q.message().id();
            fb::InlineMenu menu("pepe 1;pepe 2;pepe 3", "11;22;33");
            m.setInlineMenu(menu);
            bot.editMenu(m);
        }
    }
}