#include "User.h"

namespace MyHeatTelebot
{
    class UserSave : public MyHeatSaveInterface
    {
    public:
        User user;

        void serialize(JsonDocument &doc)
        {
            doc[F("screenType")] = static_cast<size_t>(user.screenType);
            doc[F("tempValue1")] = user.tempValue1;
            doc[F("tempValue2")] = user.tempValue2;
        }

        void deserialize(const JsonDocument &doc)
        {
            user.screenType = static_cast<ScreenType>(doc[F("screenType")].as<size_t>());
            user.tempValue1 = doc[F("tempValue1")];
            user.tempValue2 = doc[F("tempValue2")];
        }

        void manualDeserialize(const JsonDocument& doc)
        {
            deserialize(doc);
        }
    };

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
                fileName = "/users/" + fileName;
                UserSave userSave;
                MyHeatSave myHeatSave(fileName.c_str(), &userSave);
                if (myHeatSave.read())
                {
                    users[chatId] = userSave.user;
                }
            }
            file = root.openNextFile();
        }
    }

    bool isUserRegistered(const String& chatId)
    {
        return users.find(chatId) != users.end();
    }

    void registerUser(const String& chatId)
    {
        if (!isUserRegistered(chatId))
        {
            User user = {MAIN_SCREEN, 0, 0};
            users[chatId] = user;
            saveUser(chatId);
        }
    }

    void setUserScreen(const String& chatId, ScreenType type)
    {
        if (isUserRegistered(chatId))
        {
            users[chatId].screenType = type;
        }
    }

    void setUserTempValue1(const String& chatId, byte tempValue1)
    {
        if (isUserRegistered(chatId))
        {
            users[chatId].tempValue1 = tempValue1;
        }
    }

    void setUserTempValue2(const String& chatId, byte tempValue2)
    {
        if (isUserRegistered(chatId))
        {
            users[chatId].tempValue2 = tempValue2;
        }
    }

    User getUser(const String& chatId)
    {
        if (isUserRegistered(chatId))
        {
            return users[chatId];
        }
        return {MAIN_SCREEN, 0, 0}; // Return default user if not found
    }

    void saveUser(const String& chatId)
    {
        UserSave userSave;
        userSave.user = users[chatId];
        String path = "/users/" + chatId + ".json";
        MyHeatSave myHeatSave(path.c_str(), &userSave);
        myHeatSave.save();
    }

    void deleteUser(const String& chatId)
    {
        if (isUserRegistered(chatId))
        {
            String path = "/users/" + chatId + ".json";
            LittleFS.remove(path);
            users.erase(chatId);
        }
    }

    void deleteUsers()
    {
        for (auto it = users.begin(); it != users.end(); ++it)
        {
            String path = "/users/" + it->first + ".json";
            LittleFS.remove(path);
        }
        users.clear();
    }

    byte getUsersCount()
    {
        return users.size();
    }

    String getChatIdByIndex(byte index)
    {
        if (index >= users.size())
        {
            return "";
        }

        return std::next(users.begin(), index)->first;
    }
}