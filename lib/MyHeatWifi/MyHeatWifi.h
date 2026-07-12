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
    String ntpTZ; // POSIX Timezone String
    unsigned long wifiReconnectTick = 0;
    uint32_t wifiGotIpTick = 0;
    MyHeatSave *wifiData;
    bool isAPActive = false;
    bool isFallbackAPActive = false;
    bool isWebsocketClientsConnected = false;

    void serialize(JsonDocument &doc);
    void deserialize(JsonDocument &doc);

    void setAPMode();
    void setManualAPMode();
    void setFallbackAPMode();
    void setSTAMode();

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
    void setNTPSettings(String ntpServer, String ntpIANA, String ntpTZ);
    void beginNTP();

    void setWifiSettings(String wifiSSID, String wifiPassword, String apSSID, String apPassword, bool isFallbackAPEnabled, String mDNS);
    void setSSID(String ssid);
    void setPassword(String password);

    String getWifiSSID();
    String getWifiPassword();
    String getAPSSID();
    String getAPPassword();
    bool getIsFallbackAPEnabled();

    void switchWifiMode();
    void startWifiScan();
    int isScanCompleted();
    JsonDocument getNetworks();

    void setMDNS(String mDNS);
    void restartMDNS();
    String getMDNS();

    void save();

    String getNTPServer();
    String getNTPIANA();
    String getNTPTZ();

    bool isAPMode();
    String getIpAddress();
    void setIsWebsocketClientsConnected(bool isConnected);
    void manualDeserialize(JsonDocument data);
};
#endif
