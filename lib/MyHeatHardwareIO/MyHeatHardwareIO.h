#pragma once
#include <Arduino.h>
#include <U8g2lib.h>
#include "MyHeatRelay.h"
#include "MyHeatUtils.h"

constexpr byte MAX_TEMPERATURE_SCREENS = ceil(TEMPERATURE_COUNT / 3);
constexpr byte MAX_RELAY_SCREENS = RELAY_COUNT;
constexpr byte MAX_SCREENS = MAX_TEMPERATURE_SCREENS + MAX_RELAY_SCREENS;

class MyHeatHardwareIO
{
private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
    float *temperatures;
    MyHeatRelay *relays;
    byte menuIndex;
    uint32_t screenUpdateTimer;
    uint32_t screenPowerSaveTimer;
    bool isScreenOn;

    void showTemperature()
    {
        byte cursor = 24;
        for (byte i = 3 * menuIndex; i < ceil(TEMPERATURE_COUNT / (MAX_TEMPERATURE_SCREENS - menuIndex)); i++)
        {
            u8g2.print("T" + String(i) + ":");
            if (temperatures[i] == -127.00)
            {
                u8g2.print(F(" Н/Д"));
            }
            else
            {
                u8g2.print(String(temperatures[i]));
                u8g2.setFont(u8g2_font_10x20_tf);
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
        byte relayIndex = menuIndex - MAX_TEMPERATURE_SCREENS;
        u8g2.print(F("Реле "));
        u8g2.print(String(relayIndex) + ":");
        u8g2.setCursor(0, 40);
        u8g2.print("Стан: " + MyHeatUtils::getConvertedStateToText(relays[relayIndex].getMode()));
        u8g2.setCursor(0, 56);
        u8g2.print("Активне: " + MyHeatUtils::getConvertedActiveToText(relays[relayIndex].getIsActive()));
    }

public:
    MyHeatHardwareIO(float *temperatures, MyHeatRelay *relays) : u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, OLED_SCL, OLED_SDA)
    {
        this->temperatures = temperatures;
        this->relays = relays;
        this->menuIndex = 2;
        this->isScreenOn = true;
    }

    void begin()
    {
        u8g2.begin();
        u8g2.clearBuffer();
        u8g2.enableUTF8Print();
        u8g2.sendBuffer();
    }

    void tick()
    {
        if (isScreenOn && millis() - screenPowerSaveTimer >= 10000)
        {
            isScreenOn = false;
            u8g2.setPowerSave(true);
        }

        if (isScreenOn && millis() - screenUpdateTimer >= 1000)
        {
            screenUpdateTimer = millis();
            updateScreen();
        }
    }

    void updateScreen()
    {
        u8g2.clearBuffer();
        u8g2.setPowerSave(false);
        u8g2.setCursor(0, 24);

        if (menuIndex < MAX_TEMPERATURE_SCREENS)
        {
            u8g2.setFont(u8g2_font_10x20_t_cyrillic);
            showTemperature();
        }
        else if (menuIndex >= MAX_TEMPERATURE_SCREENS && menuIndex < MAX_SCREENS)
        {
            u8g2.setFont(u8g2_font_8x13_t_cyrillic);
            showRelay();
        }

        u8g2.sendBuffer();
    }
};