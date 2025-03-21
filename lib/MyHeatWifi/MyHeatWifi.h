#ifndef MYHEATWIFI_H
#define MYHEATWIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include "MyHeatSave.h"
#include "MyHeatUtils.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

class MyHeatWifi : public MyHeatSaveInterface
{
private:
    String wifiSSID;
    String wifiPassword;
    String apSSID;
    String apPassword;
    String mDNS;
    String ntpServer;
    String ntpIANA;
    int ntpOffset;
    int ntpDaylightOffset;
    unsigned long wifiReconnectTick = 0;
    unsigned long ntpSyncTick = 0;
    MyHeatSave *wifiData;
    bool isSyncTimeManually = false;

    byte wifiConnectAttemptsCount = 0;

    void serialize(JsonDocument &doc)
    {
        doc["wifi_ssid"] = wifiSSID;
        doc["wifi_password"] = wifiPassword;
        doc["ap_ssid"] = apSSID;
        doc["ap_password"] = apPassword;
        doc["mDNS"] = mDNS;
        doc["ntp_server"] = ntpServer;
        doc["ntp_iana"] = ntpIANA;
        doc["ntp_offset"] = ntpOffset;
        doc["ntp_daylight_offset"] = ntpDaylightOffset;
    }

    void deserialize(JsonDocument &doc)
    {
        wifiSSID = doc["wifi_ssid"] | STR(WIFI_SSID);
        wifiPassword = doc["wifi_password"] | STR(WIFI_PASSWORD);
        apSSID = doc["ap_ssid"] | STR(SOFTAP_SSID);
        apPassword = doc["ap_password"] | STR(SOFTAP_PASSWORD);
        mDNS = doc["mDNS"] | STR(MDNS_ADDRESS);
        ntpServer = doc["ntp_server"]| STR(NTP_SERVER);
        ntpIANA = doc["ntp_iana"] | STR(NTP_IANA);
        ntpOffset = doc["ntp_offset"] | NTP_OFFSET;
        ntpDaylightOffset = doc["ntp_daylight_offset"] | NTP_DAYLIGHT_OFFSET;
    }

    void setAPMode()
    {
        WiFi.disconnect();
        WiFi.softAP(apSSID, apPassword);
        WiFi.mode(WIFI_MODE_AP);
    }

    void setSTAMode()
    {
        WiFi.softAPdisconnect();
        WiFi.disconnect();
        WiFi.mode(WIFI_MODE_STA);
        WiFi.setSleep(false);
        if (wifiSSID != "")
        {
            WiFi.begin(wifiSSID, wifiPassword);
        }
        else
        {
            setAPMode();
        }
    }

    MyHeatWifi() {
        wifiSSID = STR(WIFI_SSID);
        wifiPassword = STR(WIFI_PASSWORD);
        apSSID = STR(SOFTAP_SSID);
        apPassword = STR(SOFTAP_PASSWORD);
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

        setSTAMode();

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
                wifiConnectAttemptsCount++;
            }

            if (wifiConnectAttemptsCount == 5)
            {
                setAPMode();
                wifiConnectAttemptsCount = 0;
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

    void setWifiSettings(String wifiSSID, String wifiPassword, String apSSID, String apPassword, String mDNS)
    {
        this->wifiSSID = wifiSSID;
        this->wifiPassword = wifiPassword;
        this->apSSID = apSSID;
        this->apPassword = apPassword;
        this->mDNS = mDNS;
        restartMDNS();
        save();
    }

    void setSSID(String ssid)
    {
        wifiSSID = ssid;
    }

    void setPassword(String password)
    {
        wifiPassword = password;
    }

    String getWifiSSID()
    {
        return wifiSSID;
    }

    String getWifiPassword()
    {
        return wifiPassword;
    }

    String getAPSSID()
    {
        return apSSID;
    }

    String getAPPassword()
    {
        return apPassword;
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
        restartMDNS();
        setSTAMode();
        beginNTP();
        save();
    }
};
#endif
