#ifndef MYHEATHARDWAREIO_H
#define MYHEATHARDWAREIO_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <EncButton.h>
#include "MyHeatTemperatures.h"
#include "MyHeatRelay.h"
#include "MyHeatUtils.h"
#include "MyHeatWifi.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

class MyHeatHardwareIO
{
private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
    MyHeatTemperatures *temperatures;
    MyHeatRelay *relays;
    EncButton eb;
    byte menuIndex;
    uint32_t screenUpdateTimer;
    uint32_t screenPowerSaveTimer;
    bool isScreenOn;

    byte maxTemperatureScreens;
    byte maxRelayScreens;
    byte maxScreens;

    void showTemperature()
    {
        byte cursor = 24;
        byte temperatureIndexStart = 3 * menuIndex;
        byte temperatureIndexEnd = min((byte)(temperatureIndexStart + 3), temperatures->getTemperatureCount());

        for (byte i = temperatureIndexStart; i < temperatureIndexEnd; i++)
        {
            float temperature = temperatures->getTemperature(i);

            u8g2.print("T" + String(i) + ":");
            if (temperature == -127.00)
            {
                u8g2.print(F(" Н/Д"));
            }
            else
            {
                u8g2.print(String(temperature));
                u8g2.setFont(u8g2_font_10x20_tn);
                u8g2.print(F("°"));
                u8g2.setFont(u8g2_font_10x20_t_cyrillic);
                u8g2.print(F("C"));
            }
            cursor += 16;
            u8g2.setCursor(0, cursor);
        }
    }

    void showRelay()
    {
        byte relayIndex = menuIndex - maxTemperatureScreens;
        u8g2.print(F("Реле "));
        u8g2.print(String(relayIndex) + ":");
        u8g2.setCursor(0, 40);
        u8g2.print("Стан: " + MyHeatUtils::getConvertedStateToText(relays[relayIndex].getMode()));
        u8g2.setCursor(0, 56);
        u8g2.print("Активне: " + MyHeatUtils::getConvertedActiveToText(relays[relayIndex].getIsActive()));
    }

    void handleEncoder()
    {
        if (eb.turn())
        {
            menuIndex = (menuIndex + eb.dir() + maxScreens) % maxScreens;

            updateScreenManual();
        }
        else if (eb.click())
        {
            updateScreenManual();
        }
        else if (eb.hold(2))
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
        u8g2.clearBuffer();
        u8g2.setPowerSave(false);
        u8g2.setCursor(0, 24);

        if (menuIndex < maxTemperatureScreens)
        {
            u8g2.setFont(u8g2_font_10x20_t_cyrillic);
            showTemperature();
        }
        else if (menuIndex >= maxTemperatureScreens && menuIndex < maxScreens)
        {
            u8g2.setFont(u8g2_font_8x13_t_cyrillic);
            showRelay();
        }

        u8g2.sendBuffer();
    }

public:
    MyHeatHardwareIO() : u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, OLED_SCL, OLED_SDA), eb(ENC_A, ENC_B, ENC_BTN)
    {
        this->menuIndex = 0;
        this->isScreenOn = true;
    }

    void begin(MyHeatTemperatures *temperatures, MyHeatRelay *relays)
    {
        this->relays = relays;
        this->temperatures = temperatures;
        u8g2.begin();
        u8g2.clearBuffer();
        u8g2.enableUTF8Print();
        u8g2.sendBuffer();
        reevaluateScreensCount();
    }

    void tick()
    {
        if (isScreenOn && millis() - screenPowerSaveTimer >= 60000)
        {
            isScreenOn = false;
            u8g2.setPowerSave(true);
        }

        if (isScreenOn && millis() - screenUpdateTimer >= 1000)
        {
            screenUpdateTimer = millis();
            updateScreen();
        }

        eb.tick();
        handleEncoder();
    }

    void reevaluateScreensCount()
    {
        maxTemperatureScreens = ceil((float) temperatures->getTemperatureCount() / 3);
        maxRelayScreens = RELAY_COUNT;
        maxScreens = maxTemperatureScreens + maxRelayScreens;
    }
};
#endif
