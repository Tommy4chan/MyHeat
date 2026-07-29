#include "User.h"

namespace MyHeatTelebot
{
    void loadUsers()
    {
        if (!LittleFS.exists("/users"))
        {
            LittleFS.mkdir("/users");
        }

        File root = LittleFS.open("/users");
        if (!root || !root.isDirectory())
        {
            Serial.println("Failed to open /users directory");
            return;
        }

        File file = root.openNextFile();
        while (file)
        {
            String fileName = file.name();
            if (fileName.endsWith(".json"))
            {
                String chatId = fileName.substring(0, fileName.length() - 5);
                registeredChatIds.push_back(chatId);
            }
            file = root.openNextFile();
        }
    }

    bool isUserRegistered(const String& chatId)
    {
        for (const auto& id : registeredChatIds)
        {
            if (id == chatId) return true;
        }
        return false;
    }

    void registerUser(const String& chatId)
    {
        if (!isUserRegistered(chatId))
        {
            registeredChatIds.push_back(chatId);

            String path = "/users/" + chatId + ".json";
            File file = LittleFS.open(path, FILE_WRITE);
            if (file)
            {
                file.print("{}");
                file.close();
            }
        }
    }

    void deleteUser(const String& chatId)
    {
        for (auto it = registeredChatIds.begin(); it != registeredChatIds.end(); ++it)
        {
            if (*it == chatId)
            {
                String path = "/users/" + chatId + ".json";
                LittleFS.remove(path);
                registeredChatIds.erase(it);
                return;
            }
        }
    }

    void deleteUsers()
    {
        for (const auto& chatId : registeredChatIds)
        {
            String path = "/users/" + chatId + ".json";
            LittleFS.remove(path);
        }
        registeredChatIds.clear();
    }

    byte getUsersCount()
    {
        return registeredChatIds.size();
    }

    String getChatIdByIndex(byte index)
    {
        if (index >= registeredChatIds.size())
        {
            return "";
        }
        return registeredChatIds[index];
    }
}