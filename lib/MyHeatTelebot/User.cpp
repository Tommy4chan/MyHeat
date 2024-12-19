#include "User.h"

namespace MyHeatTelebot
{
    class UserSave : public MyHeatSaveInterface
    {
    public:
        User user;

        void serialize(JsonDocument &doc)
        {
            doc[F("screenType")] = user.screenType;
            doc[F("isInputMode")] = user.isInputMode;
            doc[F("tempValue1")] = user.tempValue1;
            doc[F("tempValue2")] = user.tempValue2;
        }

        void deserialize(JsonDocument &doc)
        {
            user.screenType = doc[F("screenType")];
            user.isInputMode = doc[F("isInputMode")];
            user.tempValue1 = doc[F("tempValue1")];
            user.tempValue2 = doc[F("tempValue2")];
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
                MyHeatSave myHeatSave(&LittleFS, fileName.c_str(), &userSave);
                if (myHeatSave.read())
                {
                    users[chatId] = userSave.user;
                }
            }
            file = root.openNextFile();
        }
    }

    bool isUserRegistered(String chatId)
    {
        return users.find(chatId) != users.end();
    }

    bool isUserInputMode(String chatId)
    {
        if (isUserRegistered(chatId))
        {
            return users[chatId].isInputMode;
        }
        return false;
    }

    void registerUser(String chatId)
    {
        if (!isUserRegistered(chatId))
        {
            User user = {MAIN_SCREEN, false, 0, 0};
            users[chatId] = user;
            saveUser(chatId);
        }
    }

    void setUserScreen(String chatId, ScreenType type)
    {
        if (isUserRegistered(chatId))
        {
            users[chatId].screenType = type;
            saveUser(chatId);
        }
    }

    void setUserInputMode(String chatId, bool inputMode)
    {
        if (isUserRegistered(chatId))
        {
            users[chatId].isInputMode = inputMode;
            saveUser(chatId);
        }
    }

    void setUserTempValue1(String chatId, byte tempValue1)
    {
        if (isUserRegistered(chatId))
        {
            users[chatId].tempValue1 = tempValue1;
            saveUser(chatId);
        }
    }

    void setUserTempValue2(String chatId, byte tempValue2)
    {
        if (isUserRegistered(chatId))
        {
            users[chatId].tempValue2 = tempValue2;
            saveUser(chatId);
        }
    }

    User getUser(String chatId)
    {
        if (isUserRegistered(chatId))
        {
            return users[chatId];
        }
        return {MAIN_SCREEN, false, 0, 0}; // Return default user if not found
    }

    void saveUser(String chatId)
    {
        UserSave userSave;
        userSave.user = users[chatId];
        String path = "/users/" + chatId + ".json";
        MyHeatSave myHeatSave(&LittleFS, path.c_str(), &userSave);
        myHeatSave.save();
    }
}