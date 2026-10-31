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

#ifndef TELEGRAM_BOT_BOOT_NOTIFICATION
#define TELEGRAM_BOT_BOOT_NOTIFICATION false
#endif

namespace MyHeatTelebot
{
    class BotSave : public MyHeatSaveInterface
    {
    public:
        bool isEnabled = TELEGRAM_BOT_ENABLED;
        String registerPhrase = STR(REGISTER_PHRASE);
        String token = STR(TELEGRAM_BOT_TOKEN);
        bool isAlertNotificationsEnabled = TELEGRAM_BOT_ALERT_NOTIFICATIONS;
        bool isBootNotificationEnabled = TELEGRAM_BOT_BOOT_NOTIFICATION;

        void serialize(JsonDocument &doc)
        {
            doc[F("isEnabled")] = isEnabled;
            doc[F("registerPhrase")] = registerPhrase;
            doc[F("telegramBotToken")] = token;
            doc[F("isAlertNotificationsEnabled")] = isAlertNotificationsEnabled;
            doc[F("isBootNotificationEnabled")] = isBootNotificationEnabled;
        }

        void deserialize(const JsonDocument &doc)
        {
            isEnabled = doc[F("isEnabled")] | TELEGRAM_BOT_ENABLED;
            registerPhrase = doc[F("registerPhrase")] | STR(REGISTER_PHRASE);
            token = doc[F("telegramBotToken")] | STR(TELEGRAM_BOT_TOKEN);
            isAlertNotificationsEnabled = doc[F("isAlertNotificationsEnabled")] | TELEGRAM_BOT_ALERT_NOTIFICATIONS;
            isBootNotificationEnabled = doc[F("isBootNotificationEnabled")] | TELEGRAM_BOT_BOOT_NOTIFICATION;
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

    void setToken(const String& token);
    void setRegisterPhrase(const String& registerPhrase);
    void setIsEnabled(bool isEnabled);
    String getToken();
    String getRegisterPhrase();
    bool getIsEnabled();
    bool getIsAlertNotificationsEnabled();
    bool getIsBootNotificationEnabled();
    void setSettings(const String& token, const String& registerPhrase, bool isActive, bool alertNotifications, bool bootNotification);
    void sendAlertNotification(const String& message);
    void sendBootNotification();
    void manualDeserialize(const JsonDocument& data);
    
    void save();
}

#endif
