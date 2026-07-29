#ifndef MYHEATTELEBOTUSER_H
#define MYHEATTELEBOTUSER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <vector>

namespace MyHeatTelebot
{
    inline std::vector<String> registeredChatIds;

    void loadUsers();
    bool isUserRegistered(const String& chatId);
    void registerUser(const String& chatId);
    void deleteUser(const String& chatId);
    void deleteUsers();
    byte getUsersCount();
    String getChatIdByIndex(byte index);
}

#endif
