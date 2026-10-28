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
#include <QRCode.h>

#ifndef RETURN_HOME_ON_SLEEP
#define RETURN_HOME_ON_SLEEP true
#endif

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

class MyHeatHardwareIO : public MyHeatSaveInterface
{
private:
    U8G2 *u8g2;
    EncButton *eb;
    byte menuIndex;
    uint32_t screenUpdateTimer;
    uint32_t screenPowerSaveTimer;
    bool isScreenOn;

    static constexpr byte HOME_SCREENS = 1;
    byte maxTemperatureScreens;
    byte maxRelayScreens;
    byte maxScreens;

    MyHeatSave *hardwareIOData;

    byte oledSCL = OLED_SCL;
    byte oledSDA = OLED_SDA;
    byte encA = ENC_A;
    byte encB = ENC_B;
    byte encBtn = ENC_BTN;
    bool encInvert = INVERT_ENCODER;
    byte oledAddress = OLED_ADDRESS;
    int screenPowerSaveInterval = SCREEN_POWER_SAVE_INTERVAL;
    byte oledType = OLED_TYPE;
    bool isReturnHomeOnSleep = RETURN_HOME_ON_SLEEP;

    bool isModalActive = false;
    bool isEnabled = true;

    static constexpr uint32_t RELAY_EDIT_TIMEOUT = 10000;
    bool isRelayEditActive = false;
    RelayMode pendingRelayMode = RelayMode::OFF;
    uint32_t relayEditTimer = 0;

    static constexpr uint16_t SCREEN_UPDATE_INTERVAL = 1000;
    static constexpr uint16_t SCROLL_FRAME_INTERVAL = 100;
    static constexpr uint16_t SCROLL_PAUSE = 1500;
    static constexpr byte SCROLL_STEP = 2;
    static constexpr byte SCROLL_GAP = 24;
    uint32_t scrollStartTimer = 0;
    bool hasScrollingText = false;
    bool isHomeShowIp = false;

    void serialize(JsonDocument &doc) override;
    void deserialize(const JsonDocument &doc) override;

    void showHome();
    void drawScrollingText(byte x, byte y, const String &text, bool isCentered = false);
    void drawLabeledText(byte y, const String &label, const String &text);
    void showTemperature();
    void showRelay();
    void showSmokeSensor();
    void handleEncoder();
    void handleClick();
    bool isTemperatureScreen();
    bool isRelayScreen();
    byte getCurrentRelayIndex();
    void startRelayEdit();
    void applyRelayEdit();
    void cancelRelayEdit();
    void updateScreenManual();
    void showWifiSwitchModal();
    void showQRCodeModal();
    void updateScreen();
    void initOLED(byte scl, byte sda, byte address);

    TaskHandle_t loadingTaskHandle = NULL;

public:
    void animateLoadingScreen();
    void startLoadingScreen();
    void stopLoadingScreen();
    void initEncoder(byte a, byte b, byte btn, bool invert);

public:
    static MyHeatHardwareIO &getInstance()
    {
        static MyHeatHardwareIO instance;
        return instance;
    }

    MyHeatHardwareIO(const MyHeatHardwareIO &) = delete;
    MyHeatHardwareIO &operator=(const MyHeatHardwareIO &) = delete;

    MyHeatHardwareIO();
    void begin();
    void tick();
    void reevaluateScreensCount();
    void setSettings(byte oledType, byte oledAddress, byte scl, byte sda, int screenPowerSaveInterval, bool isReturnHomeOnSleep, byte encA, byte encB, byte encBtn, bool encInvert, bool isEnabled);
    
    byte getOledType();
    byte getOledAddress();
    int getScreenPowerSaveInterval();
    bool getIsReturnHomeOnSleep();
    byte getOledSCL();
    byte getOledSDA();
    byte getEncA();
    byte getEncB();
    byte getEncBtn();
    bool getEncInvert();
    bool getIsEnabled();
    bool setIsEnabled(bool isEnabled);

    void manualDeserialize(const JsonDocument& data) override;
};

#endif

