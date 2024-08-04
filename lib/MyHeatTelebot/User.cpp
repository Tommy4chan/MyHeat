#include "User.h"

namespace MyHeatTelebot
{
    bool isUserRegistered(Text chatId)
    {
        return usersDB.has(chatId);
    }

    void registerUser(Text chat_id)
    {
        User user = {MAIN_SCREEN, 0, 0};
        usersDB.set(chat_id, user);
    }
}