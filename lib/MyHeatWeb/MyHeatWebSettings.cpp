#include "MyHeatWebSettings.h"

namespace MyHeatWeb
{
    void getUsedPinsData(JsonDocument &response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        response[F("payload")][F("usedPins")][0] = myHeatDevice.getTemperaturePin();
        response[F("payload")][F("usedPins")][1] = ENC_A;
        response[F("payload")][F("usedPins")][2] = ENC_B;
        response[F("payload")][F("usedPins")][3] = ENC_BTN;
        response[F("payload")][F("usedPins")][4] = OLED_SCL;
        response[F("payload")][F("usedPins")][5] = OLED_SDA;

        for (int i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            response[F("payload")][F("usedPins")][i + 6] = myHeatDevice.getRelay(i).getPin();
        }
    }

    void setWifiSettings(JsonObject payload)
    {
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

        myHeatWifi.setWifiCredentials(payload[F("ssid")], payload[F("password")]);
    }

    void getWifiSettings(JsonDocument &response)
    {
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

        response[F("payload")][F("ssid")] = myHeatWifi.getSSID();
        response[F("payload")][F("password")] = myHeatWifi.getPassword();
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