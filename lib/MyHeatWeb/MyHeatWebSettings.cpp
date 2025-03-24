#include "MyHeatWebSettings.h"

namespace MyHeatWeb
{
    void getUsedPinsData(JsonObject response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        MyHeatHardwareIO &myHeatHardwareIO = MyHeatHardwareIO::getInstance();

        response[F("usedPins")][0] = myHeatDevice.getTemperaturePin();
        response[F("usedPins")][1] = myHeatHardwareIO.getEncA();
        response[F("usedPins")][2] = myHeatHardwareIO.getEncB();
        response[F("usedPins")][3] = myHeatHardwareIO.getEncBtn();
        response[F("usedPins")][4] = myHeatHardwareIO.getOledSCL();
        response[F("usedPins")][5] = myHeatHardwareIO.getOledSDA();
        response[F("usedPins")][6] = myHeatDevice.MyHeatSmokeSensor::getPin();

        for (int i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            response[F("usedPins")][i + 7] = myHeatDevice.getRelay(i).getPin();
        }
    }

    void setWifiSettings(JsonObject payload, JsonObject status)
    {
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

        myHeatWifi.setWifiSettings(payload[F("wifiSSID")], payload[F("wifiPassword")], payload[F("apSSID")], payload[F("apPassword")], payload[F("mDNS")]);

        setSuccessMessage(status, "Налаштування збереженні");
    }

    void getWifiSettings(JsonObject response)
    {
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

        response[F("wifiSSID")] = myHeatWifi.getWifiSSID();
        response[F("wifiPassword")] = myHeatWifi.getWifiPassword();
        response[F("apSSID")] = myHeatWifi.getAPSSID();
        response[F("apPassword")] = myHeatWifi.getAPPassword();
        response[F("mDNS")] = myHeatWifi.getMDNS();
    }

    void setNTPSettings(JsonObject payload, JsonObject status)
    {
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

        myHeatWifi.setNTPSettings(payload[F("ntpServer")], payload[F("ntpIANA")], payload[F("ntpOffset")], payload[F("ntpDaylightOffset")] ? 3600 : 0);

        setSuccessMessage(status, "Налаштування збереженні");
    }

    void getNTPSettings(JsonObject response)
    {
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

        response[F("ntpServer")] = myHeatWifi.getNTPServer();
        response[F("ntpIANA")] = myHeatWifi.getNTPIANA();
        response[F("ntpOffset")] = myHeatWifi.getNTPOffset();
        response[F("ntpDaylightOffset")] = myHeatWifi.getNTPDaylightOffset() == 3600;
    }

    void setTelegramBotSettings(JsonObject payload, JsonObject status)
    {
        MyHeatTelebot::setSettings(payload[F("token")], payload[F("registerPhrase")], payload[F("isEnabled")], payload[F("isAlertNotificationsEnabled")]);

        setSuccessMessage(status, "Налаштування збереженні");
    }

    void getTelegramBotSettings(JsonObject response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        response[F("token")] = MyHeatTelebot::getToken();
        response[F("registerPhrase")] = MyHeatTelebot::getRegisterPhrase();
        response[F("isEnabled")] = MyHeatTelebot::getIsEnabled();
        response[F("isAlertNotificationsEnabled")] = MyHeatTelebot::getIsAlertNotificationsEnabled();
    }

    void setHardwareIOSettings(JsonObject payload, JsonObject status)
    {
        MyHeatHardwareIO &myHeatHardwareIO = MyHeatHardwareIO::getInstance();

        myHeatHardwareIO.setSettings(payload[F("oledAddress")], payload[F("oledSCL")], payload[F("oledSDA")],
                                     payload[F("screenPowerSaveInterval")], payload[F("encA")], payload[F("encB")], payload[F("encBtn")],
                                     payload[F("encInvert")], payload[F("isEnabled")]);

        setSuccessMessage(status, "Налаштування збережені");
    }

    void getHardwareIOSettings(JsonObject response)
    {
        MyHeatHardwareIO &myHeatHardwareIO = MyHeatHardwareIO::getInstance();

        response[F("oledAddress")] = myHeatHardwareIO.getOledAddress();
        response[F("oledSCL")] = myHeatHardwareIO.getOledSCL();
        response[F("oledSDA")] = myHeatHardwareIO.getOledSDA();
        response[F("screenPowerSaveInterval")] = myHeatHardwareIO.getScreenPowerSaveInterval() / 1000;
        response[F("encA")] = myHeatHardwareIO.getEncA();
        response[F("encB")] = myHeatHardwareIO.getEncB();
        response[F("encBtn")] = myHeatHardwareIO.getEncBtn();
        response[F("encInvert")] = myHeatHardwareIO.getEncInvert();
        response[F("isEnabled")] = myHeatHardwareIO.getIsEnabled();
    }

    void getAllDeviceSettings(JsonObject response)
    {
        response[F("MyHeatTemperatures")] = MyHeatUtils::getFileContent("/temperatureSensors.json");
        response[F("MyHeatCustomFunctions")] = MyHeatUtils::getFileContent("/customFunctions.json");
        response[F("MyHeatRelays")] = MyHeatUtils::getFileContent("/relays.json");
        response[F("MyHeatSmokeSensor")] = MyHeatUtils::getFileContent("/smokeSensor.json");
        response[F("MyHeatHardwareIO")] = MyHeatUtils::getFileContent("/hardwareIO.json");
        response[F("MyHeatWifi")] = MyHeatUtils::getFileContent("/wifi.json");
        response[F("MyHeatTelebot")] = MyHeatUtils::getFileContent("/telebot.json");
    }

    void setAllDeviceSettings(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        MyHeatHardwareIO &myHeatHardwareIO = MyHeatHardwareIO::getInstance();
        MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

        myHeatDevice.MyHeatTemperatures::manualDeserialize(payload[F("MyHeatTemperatures")]);
        myHeatDevice.MyHeatCustomFunctions::manualDeserialize(payload[F("MyHeatCustomFunctions")]);
        myHeatDevice.MyHeatRelays::manualDeserialize(payload[F("MyHeatRelays")]);
        myHeatDevice.MyHeatSmokeSensor::manualDeserialize(payload[F("MyHeatSmokeSensor")]);
        myHeatHardwareIO.manualDeserialize(payload[F("MyHeatHardwareIO")]);
        myHeatWifi.manualDeserialize(payload[F("MyHeatWifi")]);
        MyHeatTelebot::manualDeserialize(payload[F("MyHeatTelebot")]);

        setSuccessMessage(status, "Налаштування імпортовані");
    }

    void getSmokeSensorSettings(JsonObject response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        response[F("threshold")] = myHeatDevice.MyHeatSmokeSensor::getThreshold();
        response[F("pin")] = myHeatDevice.MyHeatSmokeSensor::getPin();
        response[F("isEnabled")] = myHeatDevice.MyHeatSmokeSensor::getIsEnabled();
    }

    void setSmokeSensorSettings(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        myHeatDevice.MyHeatSmokeSensor::setSmokeSensorSettings(payload[F("threshold")], payload[F("pin")], payload[F("isEnabled")]);

        setSuccessMessage(status, "Налаштування збережені");
    }

    void getSmokeSensor(JsonObject response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        response[F("value")] = myHeatDevice.MyHeatSmokeSensor::getValue();
        response[F("isOverThreshold")] = myHeatDevice.MyHeatSmokeSensor::getIsOverThreshold();
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