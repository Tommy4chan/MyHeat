#ifndef MYHEATHARDWAREIO_H
#define MYHEATHARDWAREIO_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <EncButton.h>
#include <ArduinoJson.h>
#include "MyHeatUtils.h"
#include "MyHeatWifi.h"
#include "MyHeatDevice.h"
#include "MyHeatSave.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

class MyHeatHardwareIO : public MyHeatSaveInterface
{
private:
    //U8G2_SSD1306_128X64_NONAME_F_HW_I2C *u8g2 (for oled 0.96);
    U8G2_SH1106_128X64_NONAME_F_HW_I2C *u8g2;
    EncButton *eb;
    byte menuIndex;
    uint32_t screenUpdateTimer;
    uint32_t screenPowerSaveTimer;
    bool isScreenOn;

    byte maxTemperatureScreens;
    byte maxRelayScreens;
    byte maxScreens;

    MyHeatSave *hardwareIOData;

    byte oledSCL = OLED_SCL;
    byte oledSDA = OLED_SDA;
    byte encA = ENC_A;
    byte encB = ENC_B;
    byte encBtn = ENC_BTN;
    bool encInvert = false;
    byte oledAddress = 0x78;
    int screenPowerSaveInterval = 60000;

    bool isActive = true;

    void serialize(JsonDocument &doc)
    {
        doc["oled_scl"] = oledSCL;
        doc["oled_sda"] = oledSDA;
        doc["enc_a"] = encA;
        doc["enc_b"] = encB;
        doc["enc_btn"] = encBtn;
        doc["enc_invert"] = encInvert;
        doc["is_active"] = isActive;
        doc["oled_address"] = oledAddress;
        doc["screen_power_save_interval"] = screenPowerSaveInterval;
    }

    void deserialize(JsonDocument &doc)
    {
        oledSCL = doc["oled_scl"] | OLED_SCL;
        oledSDA = doc["oled_sda"] | OLED_SDA;
        encA = doc["enc_a"] | ENC_A;
        encB = doc["enc_b"] | ENC_B;
        encBtn = doc["enc_btn"] | ENC_BTN;
        encInvert = doc["enc_invert"] | INVERT_ENCODER;
        isActive = doc["is_active"] | HARDWARE_IO_IS_ACTIVE;
        oledAddress = doc["oled_address"] | OLED_ADDRESS;
        screenPowerSaveInterval = doc["screen_power_save_interval"] | SCREEN_POWER_SAVE_INTERVAL;
    }

    void showTemperature()
    {
        byte cursor = 24;
        byte temperatureIndexStart = 3 * menuIndex;

        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        byte temperatureIndexEnd = min((byte)(temperatureIndexStart + 3), myHeatDevice.getTemperatureCount());

        for (byte i = temperatureIndexStart; i < temperatureIndexEnd; i++)
        {
            float temperature = myHeatDevice.getTemperature(i);

            u8g2->print("T" + String(i) + ":");
            if (temperature == -127.00)
            {
                u8g2->print(F(" Н/Д"));
            }
            else
            {
                u8g2->print(String(temperature));
                u8g2->setFont(u8g2_font_10x20_tn);
                u8g2->print(F("°"));
                u8g2->setFont(u8g2_font_10x20_t_cyrillic);
                u8g2->print(F("C"));
            }
            cursor += 16;
            u8g2->setCursor(0, cursor);
        }
    }

    void showRelay()
    {
        byte relayIndex = menuIndex - maxTemperatureScreens;
        MyHeatRelay relay = MyHeatDevice::getInstance().getRelay(relayIndex);
        u8g2->print(F("Реле "));
        u8g2->print(String(relayIndex) + ":");
        u8g2->setCursor(0, 40);
        u8g2->print("Стан: " + MyHeatUtils::getConvertedStateToText(relay.getMode()));
        u8g2->setCursor(0, 56);
        u8g2->print("Активне: " + MyHeatUtils::getConvertedActiveToText(relay.getIsActive()));
    }

    void handleEncoder()
    {
        if (eb->turn())
        {
            menuIndex = (menuIndex + eb->dir() + maxScreens) % maxScreens;

            updateScreenManual();
        }
        else if (eb->click())
        {
            updateScreenManual();
        }
        else if (eb->hold(2))
        {
            MyHeatWifi::getInstance().switchWifiMode();
        }
    }

    void updateScreenManual()
    {
        isScreenOn = true;
        screenPowerSaveTimer = millis();
        updateScreen();
    }

    void updateScreen()
    {
        u8g2->clearBuffer();
        u8g2->setPowerSave(false);
        u8g2->setCursor(0, 24);

        if (menuIndex < maxTemperatureScreens)
        {
            u8g2->setFont(u8g2_font_10x20_t_cyrillic);
            showTemperature();
        }
        else if (menuIndex >= maxTemperatureScreens && menuIndex < maxScreens)
        {
            u8g2->setFont(u8g2_font_8x13_t_cyrillic);
            showRelay();
        }

        u8g2->sendBuffer();
    }

    void initOLED(byte scl, byte sda, byte address)
    {
        oledSCL = scl;
        oledSDA = sda;
        oledAddress = address;

        //u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, /* reset=*/U8X8_PIN_NONE, scl, sda)  (for oled 0.96);;
        u8g2 = new U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0, /* reset=*/U8X8_PIN_NONE, scl, sda);

        u8g2->begin();
        u8g2->setI2CAddress(address);
        u8g2->clearBuffer();
        u8g2->enableUTF8Print();
        u8g2->sendBuffer();
    }

    void initEncoder(byte a, byte b, byte btn, bool invert)
    {
        encA = a;
        encB = b;
        encBtn = btn;
        encInvert = invert;

        eb = new EncButton(encA, encB, encBtn);
        eb->setEncReverse(invert);
    }

public:
    static MyHeatHardwareIO &getInstance()
    {
        static MyHeatHardwareIO instance;
        return instance;
    }

    MyHeatHardwareIO(const MyHeatHardwareIO &) = delete;
    MyHeatHardwareIO &operator=(const MyHeatHardwareIO &) = delete;

    MyHeatHardwareIO()
    {
        menuIndex = 0;
        isScreenOn = true;
    }

    void begin()
    {
        hardwareIOData = new MyHeatSave("/hardwareIO.json", this);
        hardwareIOData->read();

        initOLED(oledSCL, oledSDA, oledAddress);
        initEncoder(encA, encB, encBtn, encInvert);

        reevaluateScreensCount();
    }

    void tick()
    {
        if (!isActive)
        {
            return;
        }

        if (isScreenOn && millis() - screenPowerSaveTimer >= screenPowerSaveInterval)
        {
            isScreenOn = false;
            u8g2->setPowerSave(true);
        }

        if (isScreenOn && millis() - screenUpdateTimer >= 1000)
        {
            screenUpdateTimer = millis();
            updateScreen();
        }

        eb->tick();
        handleEncoder();
    }

    void reevaluateScreensCount()
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        maxTemperatureScreens = ceil((float)myHeatDevice.getTemperatureCount() / 3);
        maxRelayScreens = myHeatDevice.getRelayCount();
        maxScreens = maxTemperatureScreens + maxRelayScreens;
    }

    void setSettings(byte oledAddress, byte scl, byte sda, int screenPowerSaveInterval, byte encA, byte encB, byte encBtn, bool encInvert, bool isActive)
    {
        if (setIsActive(isActive))
        {
            if (this->oledAddress != oledAddress || this->oledSCL != scl || this->oledSDA != sda)
            {
                initOLED(scl, sda, oledAddress);
            }
    
            if (this->encA != encA || this->encB != encB || this->encBtn != encBtn || this->encInvert != encInvert)
            {
                initEncoder(encA, encB, encBtn, encInvert);
            }

            this->screenPowerSaveInterval = screenPowerSaveInterval * 1000;
        }

        hardwareIOData->save();
    }

    byte getOledAddress()
    {
        return oledAddress;
    }

    int getScreenPowerSaveInterval()
    {
        return screenPowerSaveInterval;
    }

    byte getOledSCL()
    {
        return oledSCL;
    }

    byte getOledSDA()
    {
        return oledSDA;
    }

    byte getEncA()
    {
        return encA;
    }

    byte getEncB()
    {
        return encB;
    }

    byte getEncBtn()
    {
        return encBtn;
    }

    bool getEncInvert()
    {
        return encInvert;
    }

    bool getIsActive()
    {
        return isActive;
    }

    bool setIsActive(bool isActive)
    {
        this->isActive = isActive;

        if (isActive)
        {
            isScreenOn = true;
            screenPowerSaveTimer = millis();
            return true;
        }

        isScreenOn = false;
        u8g2->setPowerSave(true);
        return false;
    }
};

#endif
