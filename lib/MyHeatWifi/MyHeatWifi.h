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
    String mDNS;
    String ntpServer;
    String ntpIANA;
    int ntpOffset;
    int ntpDaylightOffset;
    unsigned long wifiReconnectTick = 0;
    unsigned long ntpSyncTick = 0;
    MyHeatSave *wifiData;
    bool isSyncTimeManually = false;

    void serialize(JsonDocument &doc)
    {
        doc["wifi_ssid"] = wifi[0];
        doc["wifi_password"] = wifi[1];
        doc["mDNS"] = mDNS;
        doc["ntp_server"] = ntpServer;
        doc["ntp_iana"] = ntpIANA;
        doc["ntp_offset"] = ntpOffset;
        doc["ntp_daylight_offset"] = ntpDaylightOffset;
    }

    void deserialize(JsonDocument &doc)
    {
        wifi[0] = doc["wifi_ssid"] | STR(WIFI_SSID);
        wifi[1] = doc["wifi_password"] | STR(WIFI_PASSWORD);
        mDNS = doc["mDNS"] | STR(MDNS_ADDRESS);
        ntpServer = doc["ntp_server"]| STR(NTP_SERVER);
        ntpIANA = doc["ntp_iana"] | STR(NTP_IANA);
        ntpOffset = doc["ntp_offset"] | NTP_OFFSET;
        ntpDaylightOffset = doc["ntp_daylight_offset"] | NTP_DAYLIGHT_OFFSET;
    }

    void setAPMode()
    {
        WiFi.disconnect();
        WiFi.softAP(STR(SOFTAP_SSID), STR(SOFTAP_PASSWORD));
        WiFi.mode(WIFI_MODE_AP);
    }

    void setSTAMode()
    {
        WiFi.softAPdisconnect();
        WiFi.disconnect();
        WiFi.begin(wifi[0], wifi[1]);
        WiFi.mode(WIFI_MODE_STA);
    }

    MyHeatWifi() {
        wifi[0] = STR(WIFI_SSID);
        wifi[1] = STR(WIFI_PASSWORD);
        mDNS = STR(MDNS_ADDRESS);
        ntpServer = STR(NTP_SERVER);
        ntpIANA = STR(NTP_IANA);
        ntpOffset = NTP_OFFSET;
        ntpDaylightOffset = NTP_DAYLIGHT_OFFSET;
    };

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
        wifiData = new MyHeatSave("/wifi.json", this);
        wifiData->read();

        if (wifi[0] != "")
        {
            WiFi.begin(wifi[0], wifi[1]);
        }
        else
        {
            setAPMode();
        }

        if (!MDNS.begin(mDNS))
            Serial.println(F("Error setting up MDNS responder!"));
        else
            Serial.println(mDNS);

            beginNTP();
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

            if (WiFi.status() == WL_CONNECTED && (millis() - ntpSyncTick >= NTP_SYNC_INTERVAL || (MyHeatUtils::isTimeDefault() && millis() - ntpSyncTick >= 10000) || isSyncTimeManually))
            {
                beginNTP();
            }
        }
    }

    bool isConnected()
    {
        return WiFi.status() == WL_CONNECTED;
    }

    void setNTPSettings(String ntpServer, String ntpIANA, int ntpOffset, int ntpDaylightOffset)
    {
        this->ntpServer = ntpServer;
        this->ntpIANA = ntpIANA;
        this->ntpOffset = ntpOffset;
        this->ntpDaylightOffset = ntpDaylightOffset;
        isSyncTimeManually = true;
        save();
    }

    void beginNTP()
    {
        Serial.println("Syncing time...");
        configTime(ntpOffset, ntpDaylightOffset, ntpServer.c_str());
        ntpSyncTick = millis();
        isSyncTimeManually = false;
    }

    void setWifiCredentials(String ssid, String password)
    {
        setSSID(ssid);
        setPassword(password);
        save();
    }

    void setWifiSettings(String ssid, String password, String mDNS)
    {
        wifi[0] = ssid;
        wifi[1] = password;
        this->mDNS = mDNS;
        restartMDNS();
        save();
    }

    void setSSID(String ssid)
    {
        wifi[0] = ssid;
    }

    void setPassword(String password)
    {
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

    void setMDNS(String mDNS)
    {
        this->mDNS = mDNS;
        restartMDNS();
        save();
    }

    void restartMDNS()
    {
        MDNS.end();
        MDNS.begin(mDNS);
    }

    String getMDNS()
    {
        return mDNS;
    }

    void save()
    {
        wifiData->save();
    }

    String getNTPServer()
    {
        return ntpServer;
    }

    String getNTPIANA()
    {
        return ntpIANA;
    }

    int getNTPOffset()
    {
        return ntpOffset;
    }

    int getNTPDaylightOffset()
    {
        return ntpDaylightOffset;
    }

    void manualDeserialize(JsonDocument data)
    {
        deserialize(data);
        save();
    }
};
#endif
