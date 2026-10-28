#ifndef MYHEATWIFI_H
#define MYHEATWIFI_H

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include <esp_sntp.h>
#include "MyHeatSave.h"
#include "MyHeatUtils.h"

#define XSTR(...) #__VA_ARGS__
#define STR(x) XSTR(x)

class MyHeatWifi : public MyHeatSaveInterface
{
private:
    String wifiSSID;
    String wifiPassword;
    String apSSID;
    String apPassword;
    bool isFallbackAPEnabled = IS_FALLBACK_AP_ENABLED;
    String mDNS;
    String ntpServer;
    String ntpIANA;
    String ntpTZ; // POSIX Timezone String
    String ntpServerAddress; // Resolved NTP server, SNTP keeps a pointer to it so it must outlive beginNTP()
    unsigned long wifiReconnectTick = 0;
    uint32_t wifiGotIpTick = 0;
    uint32_t lastNtpSyncAttempt = 0;
    MyHeatSave *wifiData;
    bool isAPActive = false;
    bool isFallbackAPActive = false;
    bool isWebsocketClientsConnected = false;

    void serialize(JsonDocument &doc) override;
    void deserialize(const JsonDocument &doc) override;

    void setAPMode();
    void setManualAPMode();
    void setFallbackAPMode();
    void setSTAMode();

    // Set from the SNTP callback, not derived from the clock: system time survives soft resets,
    // so a valid clock does not mean SNTP has finished its DNS lookup (lwIP asserts if TLS clients race it)
    static volatile bool isNtpSynced;
    static volatile time_t lastNtpSyncTime;
    volatile bool isNtpStartPending = false;
    static void onNtpSync(struct timeval *tv);

    static void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
    void handleWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);

    MyHeatWifi();

public:
    static MyHeatWifi &getInstance()
    {
        static MyHeatWifi instance;
        return instance;
    }

    MyHeatWifi(const MyHeatWifi &) = delete;
    MyHeatWifi &operator=(const MyHeatWifi &) = delete;

    void begin();
    void tick();

    bool isConnected();
    bool isTimeSynced();
    time_t getLastNtpSyncTime();
    void setNTPSettings(const String &ntpServer, const String &ntpIANA, const String &ntpTZ);
    void beginNTP();

    void setWifiSettings(const String &wifiSSID, const String &wifiPassword, const String &apSSID, const String &apPassword, bool isFallbackAPEnabled, const String &mDNS);

    String getWifiSSID();
    String getWifiPassword();
    String getAPSSID();
    String getAPPassword();
    bool getIsFallbackAPEnabled();

    void switchWifiMode();
    void startWifiScan();
    int isScanCompleted();
    JsonDocument getNetworks();

    void setMDNS(const String &mDNS);
    void restartMDNS();
    String getMDNS();

    void save();

    String getNTPServer();
    String getNTPIANA();
    String getNTPTZ();

    bool isAPMode();
    bool hasAPClients();
    String getIpAddress();
    void setIsWebsocketClientsConnected(bool isConnected);
    void manualDeserialize(const JsonDocument& data) override;
};
#endif
