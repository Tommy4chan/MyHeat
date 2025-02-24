#include "MyHeatWebSettings.h"

namespace MyHeatWeb
{
    void getUsedPinsData(JsonObject response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        response[F("usedPins")][0] = myHeatDevice.getTemperaturePin();
        response[F("usedPins")][1] = ENC_A;
        response[F("usedPins")][2] = ENC_B;
        response[F("usedPins")][3] = ENC_BTN;
        response[F("usedPins")][4] = OLED_SCL;
        response[F("usedPins")][5] = OLED_SDA;

        for (int i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            response[F("usedPins")][i + 6] = myHeatDevice.getRelay(i).getPin();
        }
    }

    void setWifiSettings(JsonObject payload)
    {
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();
        
        myHeatWifi.setWifiSettings(payload[F("ssid")], payload[F("password")], payload[F("mDNS")]);
    }

    void getWifiSettings(JsonObject response)
    {
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

        response[F("ssid")] = myHeatWifi.getSSID();
        response[F("password")] = myHeatWifi.getPassword();
        response[F("mDNS")] = myHeatWifi.getMDNS();
    }

    void setNTPSettings(JsonObject payload)
    {
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

        myHeatWifi.setNTPSettings(payload[F("ntpServer")], payload[F("ntpIANA")], payload[F("ntpOffset")], payload[F("ntpDaylightOffset")] ? 3600 : 0);
    }

    void getNTPSettings(JsonObject response)
    {
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

        response[F("ntpServer")] = myHeatWifi.getNTPServer();
        response[F("ntpIANA")] = myHeatWifi.getNTPIANA();
        response[F("ntpOffset")] = myHeatWifi.getNTPOffset();
        response[F("ntpDaylightOffset")] = myHeatWifi.getNTPDaylightOffset();
    }

    void setTelegramBotSettings(JsonObject payload)
    {
        MyHeatTelebot::setSettings(payload[F("token")], payload[F("registerPhrase")], payload[F("isActive")]);
    }

    void getTelegramBotSettings(JsonObject response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        response[F("token")] = MyHeatTelebot::getToken();
        response[F("registerPhrase")] = MyHeatTelebot::getRegisterPhrase();
        response[F("isActive")] = MyHeatTelebot::getIsActive();
    }

    void startWifiScan()
    {
        MyHeatWifi::getInstance().startWifiScan();
    }

    bool isScanCompleted()
    {
        return MyHeatWifi::getInstance().isScanCompleted() >= 0;
    }

    JsonDocument getNetworksData()
    {
        return MyHeatWifi::getInstance().getNetworks();
    }
}