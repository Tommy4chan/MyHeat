#pragma once

#include <GyverDBFile.h>
#include <LittleFS.h>

namespace MyHeatTelebot
{
    enum StateType
    {
        MAIN_SCREEN,
    };

    struct User
    {
        StateType stateType;
        byte tempValue1;
        byte tempValue2;
    };

    inline GyverDBFile usersDB(&LittleFS, "/users.bin");

    bool isUserRegistered(Text chatId);
    void registerUser(Text chat_id);
}