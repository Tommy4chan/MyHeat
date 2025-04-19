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
    bool isFallbackAPEnabled;
    String mDNS;
    String ntpServer;
    String ntpIANA;
    int ntpOffset;
    int ntpDaylightOffset;
    unsigned long wifiReconnectTick = 0;
    unsigned long ntpSyncTick = 0;
    MyHeatSave *wifiData;
    bool isSyncTimeManually = false;
    bool isAPActive = false;
    bool isFallbackAPActive = false;
    bool isWebsocketClientsConnected = false;

    byte wifiConnectAttemptsCount = 0;

    void serialize(JsonDocument &doc)
    {
        doc["wifi_ssid"] = wifiSSID;
        doc["wifi_password"] = wifiPassword;
        doc["ap_ssid"] = apSSID;
        doc["ap_password"] = apPassword;
        doc["is_fallback_ap_enabled"] = isFallbackAPEnabled;
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
        isFallbackAPEnabled = doc["is_fallback_ap_enabled"] | IS_FALLBACK_AP_ENABLED;
        mDNS = doc["mDNS"] | STR(MDNS_ADDRESS);
        ntpServer = doc["ntp_server"] | STR(NTP_SERVER);
        ntpIANA = doc["ntp_iana"] | STR(NTP_IANA);
        ntpOffset = doc["ntp_offset"] | NTP_OFFSET;
        ntpDaylightOffset = doc["ntp_daylight_offset"] | NTP_DAYLIGHT_OFFSET;
    }

    void setAPMode()
    {
        delay(100);
        WiFi.setAutoReconnect(false);
        WiFi.softAP(apSSID, apPassword);
        isAPActive = true;
    }

    void setManualAPMode()
    {
        WiFi.mode(WIFI_MODE_AP);
        setAPMode();
    }

    void setFallbackAPMode()
    {
        WiFi.mode(WIFI_MODE_APSTA);
        isFallbackAPActive = true;
        setAPMode();
    }

    void setSTAMode()
    {
        isAPActive = false;
        isFallbackAPActive = false;
        wifiConnectAttemptsCount = 0;
        WiFi.softAPdisconnect();
        WiFi.mode(WIFI_MODE_STA);
        WiFi.setSleep(false);
        if (wifiSSID != "")
        {
            WiFi.begin(wifiSSID, wifiPassword);
            WiFi.setAutoReconnect(true);
        }
        else
        {
            setAPMode();
        }
    }

    MyHeatWifi()
    {
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

        MDNS.begin(mDNS);

        beginNTP();
    }

    void tick()
    {
        if (isFallbackAPEnabled)
        {
            if (!WiFi.isConnected() && isScanCompleted() != -1 && !isAPActive && (millis() - wifiReconnectTick >= WIFI_AP_FALLBACK_TIME))
            {
                setFallbackAPMode();
                wifiConnectAttemptsCount = 0;
            }

            if (isFallbackAPActive && !isWebsocketClientsConnected && (millis() - wifiReconnectTick >= WIFI_AP_RECONNECT_INTERVAL))
            {
                WiFi.reconnect();
                wifiReconnectTick = millis();
            }

            if (isFallbackAPActive && WiFi.isConnected())
            {
                setSTAMode();
            }
        }
        if (WiFi.isConnected())
        {
            wifiReconnectTick = millis();
            if (millis() - ntpSyncTick >= NTP_SYNC_INTERVAL || (MyHeatUtils::isTimeDefault() && millis() - ntpSyncTick >= 10000) || isSyncTimeManually)
            {
                beginNTP();
            }
        }

    }

    bool isConnected()
    {
        return WiFi.isConnected();
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
        configTime(ntpOffset, ntpDaylightOffset, ntpServer.c_str());
        ntpSyncTick = millis();
        isSyncTimeManually = false;
    }

    void setWifiSettings(String wifiSSID, String wifiPassword, String apSSID, String apPassword, bool isFallbackAPEnabled, String mDNS)
    {
        this->wifiSSID = wifiSSID;
        this->wifiPassword = wifiPassword;
        this->apSSID = apSSID;
        this->apPassword = apPassword;
        this->isFallbackAPEnabled = isFallbackAPEnabled;
        this->mDNS = mDNS;

        if (!isFallbackAPEnabled)
        {
        }

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

    bool getIsFallbackAPEnabled()
    {
        return isFallbackAPEnabled;
    }

    void switchWifiMode()
    {
        if (isAPMode())
        {
            setSTAMode();
            Serial.println("Switching to STA mode");
        }
        else
        {
            setManualAPMode();
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

    bool isAPMode()
    {
        return WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA;
    }

    String getIpAddress()
    {
        if (isAPMode())
        {
            return WiFi.softAPIP().toString();
        }
        else if(WiFi.isConnected())
        {
            return WiFi.localIP().toString();
        }

        return "";
    }

    void setIsWebsocketClientsConnected(bool isConnected)
    {
        isWebsocketClientsConnected = isConnected;

        if (isConnected)
        {
            wifiReconnectTick = millis();
        }
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
