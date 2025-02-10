#ifndef MYHEATWIFI_H
#define MYHEATWIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include "MyHeatSave.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

class MyHeatWifi : public MyHeatSaveInterface
{
private:
    String wifi[2];
    unsigned long wifiReconnectTick = 0;
    unsigned long ntpSyncTick = 0;
    MyHeatSave *wifiData;

    void serialize(JsonDocument &doc)
    {
        doc["wifi_ssid"] = wifi[0];
        doc["wifi_password"] = wifi[1];
    }

    void deserialize(JsonDocument &doc)
    {
        wifi[0] = doc["wifi_ssid"].as<String>();
        wifi[1] = doc["wifi_password"].as<String>();
    }

    void setAPMode()
    {
        WiFi.disconnect();
        WiFi.softAP(F("MyHeat"), F("12345678"));
        WiFi.mode(WIFI_MODE_AP);
    }

    void setSTAMode()
    {
        WiFi.softAPdisconnect();
        WiFi.disconnect();
        WiFi.begin(wifi[0], wifi[1]);
        WiFi.mode(WIFI_MODE_STA);
    }

    MyHeatWifi() {};

public:
    static MyHeatWifi &getInstance()
    {
        static MyHeatWifi instance;
        return instance;
    }

    MyHeatWifi(const MyHeatWifi &) = delete;
    MyHeatWifi &operator=(const MyHeatWifi &) = delete;

    void begin()
    {
        wifiData = new MyHeatSave(&LittleFS, "/wifi.json", this);
        wifiData->read();

        if (wifi[0] != "")
        {
            Serial.println(wifi[0]);
            Serial.println(wifi[1]);
            WiFi.begin(wifi[0], wifi[1]);
        }
        else
        {
            setAPMode();
        }

        if (!MDNS.begin(F("esp32-myheat")))
            Serial.println(F("Error setting up MDNS responder!"));
        else
            Serial.println(F("mDNS responder started: esp32-myheat.local\n"));

        configTime(NTP_OFFSET, NTP_DAYLIGHT_OFFSET, STR(NTP_SERVER));
    }

    void tick()
    {
        if (WiFi.getMode() != WIFI_MODE_AP)
        {
            if ((WiFi.status() != WL_CONNECTED && isScanCompleted() != -1) && (millis() - wifiReconnectTick >= WIFI_RECONNECT_INTERVAL))
            {
                Serial.println("Reconnecting to WiFi...");
                WiFi.reconnect();
                wifiReconnectTick = millis();
            }

            if (WiFi.status() == WL_CONNECTED && (millis() - ntpSyncTick >= NTP_SYNC_INTERVAL || (MyHeatUtils::isTimeDefault() && millis() - ntpSyncTick >= 10000)))
            {
                Serial.println("Syncing time...");
                configTime(NTP_OFFSET, NTP_DAYLIGHT_OFFSET, STR(NTP_SERVER));
                ntpSyncTick = millis();
            }
        }
    }

    bool isConnected()
    {
        return WiFi.status() == WL_CONNECTED;
    }

    void setWifiCredentials(String ssid, String password)
    {
        setSSID(ssid);
        setPassword(password);
        wifiData->save();
    }

    void setSSID(String ssid)
    {
        Serial.println(ssid);
        wifi[0] = ssid;
    }

    void setPassword(String password)
    {
        Serial.println(password);
        wifi[1] = password;
    }

    String getSSID()
    {
        return wifi[0];
    }

    String getPassword()
    {
        return wifi[1];
    }

    void switchWifiMode()
    {
        WiFi.disconnect();
        if (WiFi.getMode() == WIFI_MODE_AP)
        {
            setSTAMode();
            Serial.println("Switching to STA mode");
        }
        else
        {
            setAPMode();
            Serial.println("Switching to AP mode");
        }
    }

    void startWifiScan()
    {
        WiFi.scanNetworks(true);
    }

    int isScanCompleted()
    {
        return WiFi.scanComplete();
    }

    JsonDocument getNetworks()
    {
        int networksCount = WiFi.scanComplete();

        JsonDocument networks;
        for (int i = 0; i < networksCount; i++)
        {
            networks[i]["ssid"] = WiFi.SSID(i);
            networks[i]["rssi"] = WiFi.RSSI(i);
        }

        WiFi.scanDelete();

        return networks;
    }
};
#endif
