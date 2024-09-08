#pragma once

#include <GyverDBFile.h>
#include <LittleFS.h>

namespace MyHeatTelebot
{
    enum ScreenType : size_t {
        MAIN_SCREEN = SH("MAIN_SCREEN"),
        TEMP_SCREEN = SH("TEMP_SCREEN"),
        RELAY_SCREEN = SH("RELAY_SCREEN"),
        FUNCTIONS_LIST_SCREEN = SH("FUNCTIONS_LIST_SCREEN"),
        FUNCTION_SCREEN = SH("FUNCTION_SCREEN"),
        FUNCTION_CHANGE_SIGN_SCREEN = SH("FUNCTION_CHANGE_SIGN_SCREEN"),
        FUNCTION_CHANGE_TEMPERATURE_SCREEN = SH("FUNCTION_CHANGE_TEMPERATURE_SCREEN"),
        FUNCTION_CHANGE_DELTA_TEMPERATURE_SCREEN = SH("FUNCTION_CHANGE_DELTA_TEMPERATURE_SCREEN"),
        FUNCTION_CHANGE_RELAY_SCREEN = SH("FUNCTION_CHANGE_RELAY_SCREEN"),
    };

    struct User
    {
        ScreenType screenType;
        bool isInputMode;
        byte tempValue1;
        byte tempValue2;
    };

    inline GyverDBFile usersDB(&LittleFS, "/users.bin");

    bool isUserRegistered(Text chatId);
    bool isUserInputMode(Text chatId);
    void registerUser(Text chat_id);
    void setUserScreen(Text chat_id, ScreenType type);
    void setUserInputMode(Text chat_id, bool inputMode);
    void setUserTempValue1(Text chat_id, byte tempValue1);
    void setUserTempValue2(Text chat_id, byte tempValue2);
    User getUser(Text chat_id);
}