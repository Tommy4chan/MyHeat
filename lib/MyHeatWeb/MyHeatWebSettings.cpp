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

        myHeatWifi.setWifiCredentials(payload[F("ssid")], payload[F("password")]);
    }

    void getWifiSettings(JsonObject response)
    {
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

        response[F("ssid")] = myHeatWifi.getSSID();
        response[F("password")] = myHeatWifi.getPassword();
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