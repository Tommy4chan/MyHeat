#ifndef MYHEATTELEBOT_H
#define MYHEATTELEBOT_H

#include <FastBot2.h>
#include "Menu.h"
#include "Message.h"
#include "User.h"
#include "MyHeatDevice.h"
#include "MyHeatUtils.h"
#include "MyHeatSave.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

namespace MyHeatTelebot
{
    class BotSave : public MyHeatSaveInterface
    {
    public:
        bool isActive = true;
        String registerPhrase = STR(REGISTER_PHRASE);
        String token = STR(TELEGRAM_BOT_TOKEN);

        void serialize(JsonDocument &doc)
        {
            doc[F("isActive")] = isActive;
            doc[F("registerPhrase")] = registerPhrase;
            doc[F("telegramBotToken")] = token;
        }

        void deserialize(JsonDocument &doc)
        {
            isActive = doc[F("isActive")];
            registerPhrase = doc[F("registerPhrase")].as<String>();
            token = doc[F("telegramBotToken")].as<String>();
        }
    };

    inline FastBot2 bot;
    inline MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
    inline MyHeatSave *botData;
    inline BotSave botSave;

    void begin();
    void tick();

    void handleUpdate(fb::Update &u);
    void handleMessage(fb::Update &u);
    void handleCommand(fb::Update &u);
    void handleQuery(fb::Update &u);
    void handleUserRegistration(fb::Update &u);
    void handleUserInput(fb::Update &u);

    void setToken(String token);
    void setRegisterPhrase(String registerPhrase);
    void setIsActive(bool isActive);
    String getToken();
    String getRegisterPhrase();
    bool getIsActive();
    void setSettings(String token, String registerPhrase, bool isActive);
    
    void save();
}

#endif
