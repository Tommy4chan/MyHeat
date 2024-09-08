#include "User.h"

namespace MyHeatTelebot
{
    bool isUserRegistered(Text chatId)
    {
        return usersDB.has(chatId);
    }

    bool isUserInputMode(Text chatId)
    {
        User user;
        usersDB[chatId].writeTo(user);
        return user.isInputMode;
    }

    void registerUser(Text chat_id)
    {
        User user = {MAIN_SCREEN, false, 0, 0};
        usersDB.set(chat_id, user);
    }

    void setUserScreen(Text chat_id, ScreenType type)
    {
        User user;
        usersDB[chat_id].writeTo(user);
        user.screenType = type;
        usersDB.set(chat_id, user);
    }

    void setUserInputMode(Text chat_id, bool inputMode)
    {
        User user;
        usersDB[chat_id].writeTo(user);
        user.isInputMode = inputMode;
        usersDB.set(chat_id, user);
    }

    void setUserTempValue1(Text chat_id, byte tempValue1)
    {
        User user;
        usersDB[chat_id].writeTo(user);
        user.tempValue1 = tempValue1;
        usersDB.set(chat_id, user);
    }

    void setUserTempValue2(Text chat_id, byte tempValue2)
    {
        User user;
        usersDB[chat_id].writeTo(user);
        user.tempValue2 = tempValue2;
        usersDB.set(chat_id, user);
    }

    User getUser(Text chat_id)
    {
        User user;
        usersDB[chat_id].writeTo(user);
        return user;
    }
}