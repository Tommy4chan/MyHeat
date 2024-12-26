#ifndef MYHEATTELEBOTUSER_H
#define MYHEATTELEBOTUSER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <map>
#include "MyHeatSave.h"
#include "StringUtils.h"

namespace MyHeatTelebot
{
    enum ScreenType : size_t {
        MAIN_SCREEN = SH("MAIN_SCREEN"),
        TEMP_SCREEN = SH("TEMP_SCREEN"),
        TEMP_DISCOVER_SCREEN = SH("TEMP_DISCOVER_SCREEN"),
        TEMP_DELETE_SCREEN = SH("TEMP_DELETE_SCREEN"),
        RELAYS_SCREEN = SH("RELAYS_SCREEN"),
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

    inline std::map<String, User> users;

    void loadUsers();
    bool isUserRegistered(String chatId);
    bool isUserInputMode(String chatId);
    void registerUser(String chatId);
    void setUserScreen(String chatId, ScreenType type);
    void setUserInputMode(String chatId, bool inputMode);
    void setUserTempValue1(String chatId, byte tempValue1);
    void setUserTempValue2(String chatId, byte tempValue2);
    User getUser(String chatId);
    void saveUser(String chatId);
}

#endif
