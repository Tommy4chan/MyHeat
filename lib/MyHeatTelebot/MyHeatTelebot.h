#ifndef MYHEATTELEBOT_H
#define MYHEATTELEBOT_H

#include <FastBot2.h>
#include "Menu.h"
#include "Message.h"
#include "User.h"
#include "MyHeatDevice.h"
#include "MyHeatUtils.h"
#include "MyHeatSave.h"
#include "MyHeatWifi.h"

#define XSTR(...) #__VA_ARGS__
#define STR(x) XSTR(x)

namespace MyHeatTelebot
{
    class BotSave : public MyHeatSaveInterface
    {
    public:
        bool isEnabled = TELEGRAM_BOT_ENABLED;
        String registerPhrase = STR(REGISTER_PHRASE);
        String token = STR(TELEGRAM_BOT_TOKEN);
        bool isAlertNotificationsEnabled = TELEGRAM_BOT_ALERT_NOTIFICATIONS;

        void serialize(JsonDocument &doc)
        {
            doc[F("isEnabled")] = isEnabled;
            doc[F("registerPhrase")] = registerPhrase;
            doc[F("telegramBotToken")] = token;
            doc[F("isAlertNotificationsEnabled")] = isAlertNotificationsEnabled;
        }

        void deserialize(const JsonDocument &doc)
        {
            isEnabled = doc[F("isEnabled")] | TELEGRAM_BOT_ENABLED;
            registerPhrase = doc[F("registerPhrase")] | STR(REGISTER_PHRASE);
            token = doc[F("telegramBotToken")] | STR(TELEGRAM_BOT_TOKEN);
            isAlertNotificationsEnabled = doc[F("isAlertNotificationsEnabled")] | TELEGRAM_BOT_ALERT_NOTIFICATIONS;
        }

        void manualDeserialize(const JsonDocument& doc)
        {
            deserialize(doc);
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

    void setToken(const String& token);
    void setRegisterPhrase(const String& registerPhrase);
    void setIsEnabled(bool isEnabled);
    String getToken();
    String getRegisterPhrase();
    bool getIsEnabled();
    bool getIsAlertNotificationsEnabled();
    void setSettings(const String& token, const String& registerPhrase, bool isActive, bool alertNotifications);
    void sendAlertNotification(const String& message);
    void manualDeserialize(const JsonDocument& data);
    
    void save();
}

#endif
