#pragma once

#include <GyverDBFile.h>
#include <LittleFS.h>

namespace MyHeatTelebot
{
    enum ScreenType : size_t {
        MAIN_SCREEN = SH("MAIN_SCREEN"),
        TEMP_SCREEN = SH("TEMP_SCREEN"),
        RELAY_SCREEN = SH("RELAY_SCREEN"),
    };

    struct User
    {
        ScreenType screenType;
        bool isInputMode;
        byte tempValue;
    };

    inline GyverDBFile usersDB(&LittleFS, "/users.bin");

    bool isUserRegistered(Text chatId);
    void registerUser(Text chat_id);
    void setUserScreen(Text chat_id, ScreenType type);
    void setUserInputMode(Text chat_id, bool inputMode);
    void setUserTempValue(Text chat_id, byte tempValue);
    User getUser(Text chat_id);
}