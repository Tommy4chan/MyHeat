#include "MyHeatWifi.h"

static constexpr uint32_t WIFI_STABILITY_DELAY_MS   = WIFI_STABILITY_DELAY;
static constexpr uint32_t WIFI_MODE_SWITCH_DELAY_MS  = WIFI_MODE_SWITCH_DELAY;
static constexpr uint32_t NTP_SYNC_RETRY_DELAY_MS    = NTP_SYNC_RETRY_DELAY;

MyHeatWifi::MyHeatWifi()
{
    wifiSSID = STR(WIFI_SSID);
    wifiPassword = STR(WIFI_PASSWORD);
    apSSID = STR(SOFTAP_SSID);
    apPassword = STR(SOFTAP_PASSWORD);
    mDNS = STR(MDNS_ADDRESS);
    deviceName = STR(DEVICE_NAME);
    ntpServer = STR(NTP_SERVER);
    ntpIANA = STR(NTP_IANA);
    ntpTZ = STR(NTP_TZ); // Default POSIX TZ string — set via NTP_TZ build flag
    wifiReconnectTick = millis();
}

void MyHeatWifi::serialize(JsonDocument &doc)
{
    doc["wifi_ssid"] = wifiSSID;
    doc["wifi_password"] = wifiPassword;
    doc["ap_ssid"] = apSSID;
    doc["ap_password"] = apPassword;
    doc["is_fallback_ap_enabled"] = isFallbackAPEnabled;
    doc["mDNS"] = mDNS;
    doc["device_name"] = deviceName;
    doc["ntp_server"] = ntpServer;
    doc["ntp_iana"] = ntpIANA;
    doc["ntp_tz"] = ntpTZ;
}

void MyHeatWifi::deserialize(const JsonDocument &doc)
{
    wifiSSID = doc["wifi_ssid"] | STR(WIFI_SSID);
    wifiPassword = doc["wifi_password"] | STR(WIFI_PASSWORD);
    apSSID = doc["ap_ssid"] | STR(SOFTAP_SSID);
    apPassword = doc["ap_password"] | STR(SOFTAP_PASSWORD);
    isFallbackAPEnabled = doc["is_fallback_ap_enabled"] | IS_FALLBACK_AP_ENABLED;
    mDNS = doc["mDNS"] | STR(MDNS_ADDRESS);
    isDeviceNameStored = !doc["device_name"].isNull();
    deviceName = doc["device_name"] | STR(DEVICE_NAME);
    ntpServer = doc["ntp_server"] | STR(NTP_SERVER);
    ntpIANA = doc["ntp_iana"] | STR(NTP_IANA);
    ntpTZ = doc["ntp_tz"] | STR(NTP_TZ);
}

void MyHeatWifi::setAPMode()
{
    delay(WIFI_MODE_SWITCH_DELAY_MS);
    WiFi.setAutoReconnect(false);
    WiFi.softAP(apSSID.c_str(), apPassword.c_str());
    isAPActive = true;
}

void MyHeatWifi::setManualAPMode()
{
    WiFi.mode(WIFI_MODE_AP);
    setAPMode();
}

void MyHeatWifi::setFallbackAPMode()
{
    WiFi.mode(WIFI_MODE_APSTA);
    isFallbackAPActive = true;

    // In Fallback mode, we start AP but keep auto-reconnect ON so STA tries to connect in background
    delay(WIFI_MODE_SWITCH_DELAY_MS);
    WiFi.setAutoReconnect(true);
    WiFi.softAP(apSSID.c_str(), apPassword.c_str());
    isAPActive = true;
}

void MyHeatWifi::setSTAMode()
{
    isAPActive = false;
    isFallbackAPActive = false;
    wifiReconnectTick = millis();
    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_MODE_STA);
    WiFi.setSleep(false);
    if (wifiSSID != "")
    {
        WiFi.begin(wifiSSID.c_str(), wifiPassword.c_str());
        WiFi.setAutoReconnect(true);
    }
    else
    {
        setManualAPMode();
    }
}

void MyHeatWifi::onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
    MyHeatWifi::getInstance().handleWiFiEvent(event, info);
}

void MyHeatWifi::handleWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info)
{
    switch (event)
    {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
        Serial.print("WiFi Connected. IP: ");
        Serial.println(WiFi.localIP());
        if (isFallbackAPActive)
        {
            setSTAMode();
        }
        wifiGotIpTick = millis();
        isNtpStartPending = true;
        break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
        Serial.println("WiFi Disconnected.");
        wifiGotIpTick = 0;
        break;
    default:
        break;
    }
}

void MyHeatWifi::begin()
{
    wifiData = new MyHeatSave("/wifi.json", this);
    wifiData->read();

    WiFi.onEvent(onWiFiEvent);
    sntp_set_time_sync_notification_cb(onNtpSync);

    setSTAMode();
    MDNS.begin(mDNS.c_str());
    MyHeatDiscovery::advertise(deviceName);
    // Timezone is applied right away so time kept across soft resets is shown as local before NTP starts
    setenv("TZ", ntpTZ.c_str(), 1);
    tzset();
    // NTP is started on GOT_IP: starting it before there is an IP fails the DNS lookup and lwIP backs off for 15s+
}

void MyHeatWifi::tick()
{
    if (isFallbackAPEnabled)
    {
        if (!WiFi.isConnected() && isScanCompleted() != -1 && !isAPActive && (millis() - wifiReconnectTick >= WIFI_AP_FALLBACK_TIME))
        {
            setFallbackAPMode();
        }

        // We temporarily disable auto-reconnect while websockets are connected to prevent AP lag
        if (isFallbackAPActive)
        {
            if (isWebsocketClientsConnected)
            {
                if (WiFi.getAutoReconnect())
                {
                    WiFi.setAutoReconnect(false);
                }
            }
            else
            {
                if (!WiFi.getAutoReconnect())
                {
                    WiFi.setAutoReconnect(true);
                    WiFi.reconnect(); // Kickoff reconnection
                }
            }
        }
    }
    // Started here rather than in the WiFi event task to keep SNTP restarts on the main loop
    if (isNtpStartPending && WiFi.isConnected())
    {
        isNtpStartPending = false;
        beginNTP();
    }

    if (isConnected() && !isNtpSynced)
    {
        if (millis() - lastNtpSyncAttempt > NTP_SYNC_RETRY_DELAY_MS)
        {
            beginNTP();
        }
    }
}

volatile bool MyHeatWifi::isNtpSynced = false;
volatile time_t MyHeatWifi::lastNtpSyncTime = 0;

void MyHeatWifi::onNtpSync(struct timeval *tv)
{
    lastNtpSyncTime = tv ? tv->tv_sec : time(nullptr);
    isNtpSynced = true;
}

bool MyHeatWifi::isTimeSynced()
{
    return isNtpSynced;
}

time_t MyHeatWifi::getLastNtpSyncTime()
{
    return lastNtpSyncTime;
}

bool MyHeatWifi::isConnected()
{
    return WiFi.isConnected() && wifiGotIpTick != 0 && (millis() - wifiGotIpTick > WIFI_STABILITY_DELAY_MS);
}

void MyHeatWifi::setNTPSettings(const String &ntpServer, const String &ntpIANA, const String &ntpTZ)
{
    this->ntpServer = ntpServer;
    this->ntpIANA = ntpIANA;
    this->ntpTZ = ntpTZ;
    save();
    beginNTP();
}

void MyHeatWifi::beginNTP()
{
    lastNtpSyncAttempt = millis();
    isNtpSynced = false;

    // Resolve the NTP host here on the main loop and hand SNTP a literal IP, so SNTP never runs its own
    // DNS lookup: that lookup is slow on boot and races TLS clients into the lwIP sys_untimeout assert
    IPAddress serverIp;
    if (!WiFi.isConnected() || !WiFi.hostByName(ntpServer.c_str(), serverIp))
    {
        // Not falling back to the host name, tick() retries after NTP_SYNC_RETRY_DELAY
        return;
    }
    ntpServerAddress = serverIp.toString();

    Serial.println("NTP sync started with TZ: " + ntpTZ);
    configTzTime(ntpTZ.c_str(), ntpServerAddress.c_str());
}

void MyHeatWifi::setWifiSettings(const String &wifiSSID, const String &wifiPassword, const String &apSSID, const String &apPassword, bool isFallbackAPEnabled, const String &mDNS, const String &deviceName)
{
    this->deviceName = deviceName.length() > 0 ? deviceName : String(STR(DEVICE_NAME));
    isDeviceNameStored = true;
    this->apSSID = apSSID;
    this->apPassword = apPassword;
    this->isFallbackAPEnabled = isFallbackAPEnabled;
    this->mDNS = mDNS;

    if (this->wifiSSID != wifiSSID || this->wifiPassword != wifiPassword)
    {
        this->wifiSSID = wifiSSID;
        this->wifiPassword = wifiPassword;
        setSTAMode();
    }

    restartMDNS();
    save();
}

String MyHeatWifi::getWifiSSID()
{
    return wifiSSID;
}

String MyHeatWifi::getWifiPassword()
{
    return wifiPassword;
}

String MyHeatWifi::getAPSSID()
{
    return apSSID;
}

String MyHeatWifi::getAPPassword()
{
    return apPassword;
}

bool MyHeatWifi::getIsFallbackAPEnabled()
{
    return isFallbackAPEnabled;
}

void MyHeatWifi::switchWifiMode()
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

void MyHeatWifi::startWifiScan()
{
    WiFi.scanNetworks(true);
}

int MyHeatWifi::isScanCompleted()
{
    return WiFi.scanComplete();
}

JsonDocument MyHeatWifi::getNetworks()
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

void MyHeatWifi::setMDNS(const String &mDNS)
{
    this->mDNS = mDNS;
    restartMDNS();
    save();
}

void MyHeatWifi::restartMDNS()
{
    MDNS.end();
    MDNS.begin(mDNS.c_str());
    MyHeatDiscovery::advertise(deviceName);
}

String MyHeatWifi::getMDNS()
{
    return mDNS;
}

String MyHeatWifi::getDeviceName()
{
    return deviceName;
}

void MyHeatWifi::setDeviceName(const String &deviceName)
{
    this->deviceName = deviceName.length() > 0 ? deviceName : String(STR(DEVICE_NAME));
    isDeviceNameStored = true;
    restartMDNS();
    save();
}

bool MyHeatWifi::hasStoredDeviceName()
{
    return isDeviceNameStored;
}

void MyHeatWifi::save()
{
    wifiData->save();
}

String MyHeatWifi::getNTPServer()
{
    return ntpServer;
}

String MyHeatWifi::getNTPIANA()
{
    return ntpIANA;
}

String MyHeatWifi::getNTPTZ()
{
    return ntpTZ;
}

bool MyHeatWifi::isAPMode()
{
    return WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA;
}

bool MyHeatWifi::hasAPClients()
{
    return WiFi.softAPgetStationNum() > 0;
}

String MyHeatWifi::getIpAddress()
{
    if (isAPMode())
    {
        return WiFi.softAPIP().toString();
    }
    else if (WiFi.isConnected())
    {
        return WiFi.localIP().toString();
    }
    return "";
}

void MyHeatWifi::setIsWebsocketClientsConnected(bool isConnected)
{
    isWebsocketClientsConnected = isConnected;
    if (isConnected)
    {
        wifiReconnectTick = millis();
    }
}

void MyHeatWifi::manualDeserialize(const JsonDocument& data)
{
    String oldSSID = wifiSSID;
    String oldPassword = wifiPassword;
    deserialize(data);
    restartMDNS();
    save();

    // Reconnecting drops the websocket that carried the import, so only do it when the network changed
    if (wifiSSID != oldSSID || wifiPassword != oldPassword || isAPActive || isFallbackAPActive)
    {
        setSTAMode();
    }
    // NTP resolves its server over DNS, that has to happen on the main loop, not in the web task
    isNtpStartPending = true;
}
