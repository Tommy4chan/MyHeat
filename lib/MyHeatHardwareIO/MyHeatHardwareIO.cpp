#include "MyHeatHardwareIO.h"

void MyHeatHardwareIO::serialize(JsonDocument &doc)
{
    doc["oledType"] = oledType;
    doc["oledSCL"] = oledSCL;
    doc["oledSDA"] = oledSDA;
    doc["encA"] = encA;
    doc["encB"] = encB;
    doc["encBtn"] = encBtn;
    doc["encInvert"] = encInvert;
    doc["isEnabled"] = isEnabled;
    doc["oledAddress"] = oledAddress;
    doc["screenPowerSaveInterval"] = screenPowerSaveInterval;
    doc["isReturnHomeOnSleep"] = isReturnHomeOnSleep;
}

void MyHeatHardwareIO::deserialize(const JsonDocument &doc)
{
    oledType = doc["oledType"] | OLED_TYPE;
    oledSCL = doc["oledSCL"] | OLED_SCL;
    oledSDA = doc["oledSDA"] | OLED_SDA;
    encA = doc["encA"] | ENC_A;
    encB = doc["encB"] | ENC_B;
    encBtn = doc["encBtn"] | ENC_BTN;
    encInvert = doc["encInvert"] | INVERT_ENCODER;
    isEnabled = doc["isEnabled"] | HARDWARE_IO_IS_ENABLED;
    oledAddress = doc["oledAddress"] | OLED_ADDRESS;
    screenPowerSaveInterval = doc["screenPowerSaveInterval"] | SCREEN_POWER_SAVE_INTERVAL;
    isReturnHomeOnSleep = doc["isReturnHomeOnSleep"] | RETURN_HOME_ON_SLEEP;
}

void MyHeatHardwareIO::showHome()
{
    struct tm timeinfo;
    bool isTimeSynced = getLocalTime(&timeinfo, 0) && timeinfo.tm_year >= 100;

    char timeBuffer[12];
    char dateBuffer[12];
    if (isTimeSynced)
    {
        strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeinfo);
        strftime(dateBuffer, sizeof(dateBuffer), "%d.%m.%Y", &timeinfo);
    }
    else
    {
        strcpy(timeBuffer, "--:--:--");
        strcpy(dateBuffer, "--.--.----");
    }
    drawScrollingText(0, LINE_Y[0], timeBuffer, true);
    drawScrollingText(0, LINE_Y[1], dateBuffer, true);

    MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();
    String ipAddress = myHeatWifi.getIpAddress();

    if (myHeatWifi.isAPMode())
    {
        drawLabeledText(LINE_Y[2], F("AP: "), myHeatWifi.getAPSSID());
    }
    else if (ipAddress != "")
    {
        drawLabeledText(LINE_Y[2], F("WiFi: "), myHeatWifi.getWifiSSID());
    }
    else
    {
        drawLabeledText(LINE_Y[2], F("WiFi: "), F("відключено"));
    }

    String mDNS = myHeatWifi.getMDNS();
    if (ipAddress == "")
    {
        drawScrollingText(0, LINE_Y[3], F("Немає адреси"), true);
    }
    else if (mDNS != "" && !isHomeShowIp)
    {
        drawScrollingText(0, LINE_Y[3], mDNS + ".local", true);
    }
    else
    {
        drawScrollingText(0, LINE_Y[3], ipAddress, true);
    }
}

void MyHeatHardwareIO::drawLabeledText(byte y, const String &label, const String &text, bool isCentered)
{
    String fullText = label + text;
    if (u8g2->getUTF8Width(fullText.c_str()) <= u8g2->getDisplayWidth())
    {
        drawScrollingText(0, y, fullText, isCentered);
        return;
    }

    u8g2->drawUTF8(0, y, label.c_str());
    drawScrollingText(u8g2->getUTF8Width(label.c_str()), y, text);
}

void MyHeatHardwareIO::drawScrollingText(byte x, byte y, const String &text, bool isCentered)
{
    int16_t areaWidth = u8g2->getDisplayWidth() - x;
    int16_t textWidth = u8g2->getUTF8Width(text.c_str());

    if (textWidth <= areaWidth)
    {
        u8g2->drawUTF8(isCentered ? x + (areaWidth - textWidth) / 2 : x, y, text.c_str());
        return;
    }

    // Marquee loop: pause at the start, then scroll until the second copy reaches x = 0
    hasScrollingText = true;
    int16_t period = textWidth + SCROLL_GAP;
    uint32_t cycleDuration = SCROLL_PAUSE + (uint32_t)period * SCROLL_FRAME_INTERVAL / SCROLL_STEP;
    uint32_t elapsed = (millis() - scrollStartTimer) % cycleDuration;
    int16_t offset = elapsed < SCROLL_PAUSE ? 0 : (elapsed - SCROLL_PAUSE) / SCROLL_FRAME_INTERVAL * SCROLL_STEP;

    // Clip to the text area so scrolled text does not overlap anything drawn to the left of it
    u8g2->setClipWindow(x, 0, u8g2->getDisplayWidth(), u8g2->getDisplayHeight());
    u8g2->drawUTF8(x - offset, y, text.c_str());
    u8g2->drawUTF8(x + period - offset, y, text.c_str());
    u8g2->setMaxClipWindow();
}

void MyHeatHardwareIO::showTemperature()
{
    byte temperatureIndexStart = TEMPERATURES_PER_SCREEN * (menuIndex - HOME_SCREENS);

    MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
    byte temperatureIndexEnd = min((byte)(temperatureIndexStart + TEMPERATURES_PER_SCREEN), myHeatDevice.temperatures.getTemperatureCount());

    for (byte i = temperatureIndexStart; i < temperatureIndexEnd; i++)
    {
        float temperature = myHeatDevice.temperatures.getTemperature(i);
        u8g2->setCursor(0, LARGE_LINE_Y[i - temperatureIndexStart]);

        u8g2->print("T" + String(i) + ":");
        if (temperature == -127.00)
        {
            u8g2->print(F(" Н/Д"));
        }
        else
        {
            u8g2->print(String(temperature));
            u8g2->setFont(u8g2_font_10x20_tf); // The cyrillic and numeric 10x20 fonts have no degree sign
            u8g2->print(F("°"));
            u8g2->setFont(u8g2_font_10x20_t_cyrillic);
            u8g2->print(F("C"));
        }
    }
}

void MyHeatHardwareIO::showRelay()
{
    byte relayIndex = getCurrentRelayIndex();
    MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
    MyHeatRelay relay = myHeatDevice.relays.getRelay(relayIndex);
    u8g2->setCursor(0, LINE_Y[0]);
    u8g2->print(F("Реле "));
    u8g2->print(String(relayIndex) + ":");
    if (isRelayEditActive)
    {
        u8g2->print(F(" зміна"));
    }
    u8g2->setCursor(0, LINE_Y[1]);
    u8g2->print(F("Стан: "));
    if (isRelayEditActive)
    {
        String modeText = MyHeatUtils::getConvertedStateToText(static_cast<byte>(pendingRelayMode));
        u8g2_uint_t x = u8g2->getUTF8Width("Стан: ");
        u8g2->drawBox(x - 1, LINE_Y[1] - u8g2->getAscent() - 1, u8g2->getUTF8Width(modeText.c_str()) + 2, u8g2->getMaxCharHeight());
        u8g2->setFontMode(1);
        u8g2->setDrawColor(0);
        u8g2->print(modeText);
        u8g2->setDrawColor(1);
        u8g2->setFontMode(0);
    }
    else
    {
        u8g2->print(MyHeatUtils::getConvertedStateToText(static_cast<byte>(relay.getMode())));
    }
    u8g2->setCursor(0, LINE_Y[2]);
    u8g2->print("Активне: " + MyHeatUtils::getConvertedActiveToText(relay.getIsActive()));

    // What decides the relay state: manual mode, the function that won in auto, or nothing
    String controller;
    int8_t functionIndex = relay.getControllingFunction();
    if (relay.getMode() != RelayMode::AUTO)
    {
        controller = F("вручну");
    }
    else if (functionIndex >= 0 && functionIndex < myHeatDevice.customFunctions.getCustomFunctionCount())
    {
        const char *name = myHeatDevice.customFunctions.getCustomFunction(functionIndex).getName();
        controller = strlen(name) > 0 ? String(name) : "Функція " + String(functionIndex);
    }
    else
    {
        controller = F("немає");
    }
    drawLabeledText(LINE_Y[3], F("Керує: "), controller, false); // Left-aligned like the other relay lines
}

void MyHeatHardwareIO::showSmokeSensor()
{
    MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
    u8g2->setCursor(0, LINE_Y[0]);
    u8g2->print(F("Датчик диму: "));
    u8g2->setCursor(0, LINE_Y[1]);
    u8g2->print(myHeatDevice.smokeSensor.getValue() == -1 ? "Ініціалізація" : String(myHeatDevice.smokeSensor.getValue()));
    u8g2->setCursor(0, LINE_Y[2]);
    u8g2->print("Небезпечно: " + MyHeatUtils::getConvertedActiveToText(myHeatDevice.smokeSensor.getIsOverThreshold()));
    u8g2->setCursor(0, LINE_Y[3]);
    u8g2->print("Поріг: " + String(myHeatDevice.smokeSensor.getThreshold()));
}

void MyHeatHardwareIO::handleEncoder()
{
    if (eb->turn())
    {
        // First turn on a sleeping screen only wakes it, so the user sees where they are before navigating
        if (!isScreenOn)
        {
            // Sleep may have reset the screen to home, restart scrolling from the beginning
            scrollStartTimer = millis();
        }
        else if (isRelayEditActive)
        {
            pendingRelayMode = static_cast<RelayMode>((static_cast<byte>(pendingRelayMode) + eb->dir() + 3) % 3);
            relayEditTimer = millis();
        }
        else
        {
            menuIndex = (menuIndex + eb->dir() + maxScreens) % maxScreens;
            scrollStartTimer = millis();
        }
        updateScreenManual();
    }
    else if (eb->click())
    {
        // Click on a sleeping screen or an open modal only wakes / closes it
        if (isScreenOn && !isModalActive)
        {
            handleClick();
        }
        updateScreenManual();
    }
    else if (eb->hold(1))
    {
        cancelRelayEdit();
        MyHeatWifi::getInstance().switchWifiMode();
        showWifiSwitchModal();
    }
    else if (!isModalActive && eb->holdFor(5000))
    {
        cancelRelayEdit();
        showQRCodeModal();
    }
}

void MyHeatHardwareIO::handleClick()
{
    if (menuIndex < HOME_SCREENS)
    {
        isHomeShowIp = !isHomeShowIp;
    }
    else if (isRelayScreen())
    {
        isRelayEditActive ? applyRelayEdit() : startRelayEdit();
    }
}

bool MyHeatHardwareIO::isTemperatureScreen()
{
    return menuIndex >= HOME_SCREENS && menuIndex < HOME_SCREENS + maxTemperatureScreens;
}

bool MyHeatHardwareIO::isRelayScreen()
{
    byte relayScreensStart = HOME_SCREENS + maxTemperatureScreens;
    return menuIndex >= relayScreensStart && menuIndex < relayScreensStart + maxRelayScreens;
}

byte MyHeatHardwareIO::getCurrentRelayIndex()
{
    return menuIndex - HOME_SCREENS - maxTemperatureScreens;
}

void MyHeatHardwareIO::startRelayEdit()
{
    isRelayEditActive = true;
    pendingRelayMode = MyHeatDevice::getInstance().relays.getRelay(getCurrentRelayIndex()).getMode();
    relayEditTimer = millis();
}

void MyHeatHardwareIO::applyRelayEdit()
{
    isRelayEditActive = false;

    MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
    byte relayIndex = getCurrentRelayIndex();

    if (relayIndex >= myHeatDevice.relays.getRelayCount() || myHeatDevice.relays.getRelay(relayIndex).getMode() == pendingRelayMode)
    {
        return;
    }

    myHeatDevice.relays.setRelayMode(relayIndex, pendingRelayMode);
    myHeatDevice.manualTick();
}

void MyHeatHardwareIO::cancelRelayEdit()
{
    isRelayEditActive = false;
}

void MyHeatHardwareIO::updateScreenManual()
{
    isModalActive = false;
    isScreenOn = true;
    screenPowerSaveTimer = millis();
    updateScreen();
}

void MyHeatHardwareIO::showWifiSwitchModal()
{
    u8g2->clearBuffer();
    isModalActive = true;

    MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

    u8g2->setFont(u8g2_font_8x13_t_cyrillic);
    u8g2->setCursor(0, 24);
    if (myHeatWifi.isAPMode()) {
        u8g2->print("Точка доступу");
        u8g2->setCursor(0, 40);
        u8g2->print(myHeatWifi.getAPSSID());
        u8g2->setCursor(0, 56);
        u8g2->print(myHeatWifi.getAPPassword());
    }
    else {
        u8g2->print("WiFi мережа");
        u8g2->setCursor(0, 40);
        u8g2->print("активна");
    }

    u8g2->sendBuffer();
}

void MyHeatHardwareIO::showQRCodeModal()
{
    u8g2->clearBuffer();
    isModalActive = true;
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(2)];

    String localIP = MyHeatWifi::getInstance().getIpAddress();

    if (localIP != "")
    {
        String webUIURL = "http://" + localIP + "/";
        qrcode_initText(&qrcode, qrcodeData, 2, ECC_LOW, webUIURL.c_str());

        uint8_t scale = 2;
        int xOffset = (128 - qrcode.size * scale) / 2;  
        int yOffset = (64 - qrcode.size * scale) / 2;   
      
        for (uint8_t y = 0; y < qrcode.size; y++)
        {
            for (uint8_t x = 0; x < qrcode.size; x++)
            {
                if (qrcode_getModule(&qrcode, x, y))
                {
                    u8g2->drawBox(xOffset + x * scale, yOffset + y * scale, scale, scale);
                }
            }
        }
    }
    else
    {
        u8g2->setFont(u8g2_font_8x13_t_cyrillic);
        u8g2->setCursor(0, 24);
        u8g2->print("Нема підключення");
        u8g2->setCursor(0, 40);
        u8g2->print("до інтернету(");
    }

    u8g2->sendBuffer();
}

void MyHeatHardwareIO::updateScreen()
{
    u8g2->clearBuffer();
    u8g2->setPowerSave(false);
    hasScrollingText = false;

    if (menuIndex < HOME_SCREENS)
    {
        u8g2->setFont(u8g2_font_8x13_t_cyrillic);
        showHome();
    }
    else if (isTemperatureScreen())
    {
        u8g2->setFont(u8g2_font_10x20_t_cyrillic);
        showTemperature();
    }
    else if (isRelayScreen())
    {
        u8g2->setFont(u8g2_font_8x13_t_cyrillic);
        showRelay();
    }
    else if (menuIndex < maxScreens)
    {
        u8g2->setFont(u8g2_font_8x13_t_cyrillic);
        showSmokeSensor();
    }

    u8g2->sendBuffer();
}

void MyHeatHardwareIO::initOLED(byte scl, byte sda, byte address)
{
    oledSCL = scl;
    oledSDA = sda;
    oledAddress = address;

    if (u8g2)
    {
        delete u8g2;
        u8g2 = nullptr;
    }

    if (oledType == 1) {
        u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, /* reset=*/U8X8_PIN_NONE, scl, sda);
    } else {
        u8g2 = new U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0, /* reset=*/U8X8_PIN_NONE, scl, sda);
    }
    
    u8g2->setI2CAddress(address);
    u8g2->begin();
    u8g2->clearBuffer();
    u8g2->enableUTF8Print();
    u8g2->sendBuffer();

    // begin() powers the panel on, restore sleep if the screen was off
    if (!isScreenOn || !isEnabled)
    {
        u8g2->setPowerSave(true);
    }

    isModalActive = false;
}

void MyHeatHardwareIO::initEncoder(byte a, byte b, byte btn, bool invert)
{
    encA = a;
    encB = b;
    encBtn = btn;
    encInvert = invert;

    if (eb)
    {
        delete eb;
        eb = nullptr;
    }

    eb = new EncButton(encA, encB, encBtn);
    eb->setEncReverse(invert);
}

MyHeatHardwareIO::MyHeatHardwareIO()
{
    u8g2 = nullptr;
    eb = nullptr;
    menuIndex = 0;
    isScreenOn = true;
}

void loadingTask(void *pvParameters)
{
    MyHeatHardwareIO *io = (MyHeatHardwareIO *)pvParameters;
    io->animateLoadingScreen();
}

void MyHeatHardwareIO::animateLoadingScreen()
{
    uint8_t dots = 0;
    while (true)
    {
        if (u8g2)
        {
            u8g2->clearBuffer();
            u8g2->setFont(u8g2_font_8x13_t_cyrillic);
            u8g2->setCursor(0, 35);
            u8g2->print("Завантаження");
            for(uint8_t i = 0; i < dots; i++) {
                u8g2->print(".");
            }
            u8g2->sendBuffer();
        }
        dots = (dots + 1) % 4;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void MyHeatHardwareIO::startLoadingScreen()
{
    if (isEnabled && u8g2)
    {
        xTaskCreatePinnedToCore(
            loadingTask,
            "LoadingTask",
            2048,
            this,
            1,
            &loadingTaskHandle,
            0
        );
    }
}

void MyHeatHardwareIO::stopLoadingScreen()
{
    if (loadingTaskHandle != NULL)
    {
        vTaskDelete(loadingTaskHandle);
        loadingTaskHandle = NULL;
        
        if (u8g2) {
            u8g2->clearBuffer();
            u8g2->sendBuffer();
        }
    }
}

void MyHeatHardwareIO::begin()
{
    hardwareIOData = new MyHeatSave("/hardwareIO.json", this);
    hardwareIOData->read();

    initOLED(oledSCL, oledSDA, oledAddress);
    initEncoder(encA, encB, encBtn, encInvert);

    reevaluateScreensCount();
}

void MyHeatHardwareIO::tick()
{
    if (!isEnabled)
    {
        return;
    }

    if (isScreenOn && millis() - screenPowerSaveTimer >= screenPowerSaveInterval)
    {
        isScreenOn = false;
        isModalActive = false;
        cancelRelayEdit();
        u8g2->setPowerSave(true);

        if (isReturnHomeOnSleep)
        {
            menuIndex = 0;
            scrollStartTimer = millis();
        }
    }

    if (isRelayEditActive && millis() - relayEditTimer >= RELAY_EDIT_TIMEOUT)
    {
        cancelRelayEdit();
        updateScreen();
    }

    uint16_t updateInterval = hasScrollingText ? SCROLL_FRAME_INTERVAL : SCREEN_UPDATE_INTERVAL;
    if (isScreenOn && !isModalActive && millis() - screenUpdateTimer >= updateInterval)
    {
        screenUpdateTimer = millis();
        updateScreen();
    }

    if (eb)
    {
        eb->tick();
    }
    handleEncoder();
}

void MyHeatHardwareIO::reevaluateScreensCount()
{
    MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

    maxTemperatureScreens = ceil((float)myHeatDevice.temperatures.getTemperatureCount() / TEMPERATURES_PER_SCREEN);
    maxRelayScreens = myHeatDevice.relays.getRelayCount();
    maxScreens = HOME_SCREENS + maxTemperatureScreens + maxRelayScreens + 1;

    // Screen layout changed, the edited relay may no longer be under the cursor
    cancelRelayEdit();
    if (menuIndex >= maxScreens)
    {
        menuIndex = 0;
    }
}

void MyHeatHardwareIO::setSettings(byte type, byte address, byte scl, byte sda, int powerSaveInterval, bool isReturnHomeOnSleep, byte a, byte b, byte btn, bool invert, bool isEnabled)
{
    if (setIsEnabled(isEnabled))
    {
        if (this->oledType != type || this->oledAddress != address || this->oledSCL != scl || this->oledSDA != sda)
        {
            this->oledType = type;
            initOLED(scl, sda, address);
        }

        if (this->encA != a || this->encB != b || this->encBtn != btn || this->encInvert != invert)
        {
            initEncoder(a, b, btn, invert);
        }

        this->screenPowerSaveInterval = powerSaveInterval * 1000;
        this->isReturnHomeOnSleep = isReturnHomeOnSleep;
    }

    hardwareIOData->save();
}

byte MyHeatHardwareIO::getOledType()
{
    return oledType;
}

byte MyHeatHardwareIO::getOledAddress()
{
    return oledAddress;
}

int MyHeatHardwareIO::getScreenPowerSaveInterval()
{
    return screenPowerSaveInterval;
}

byte MyHeatHardwareIO::getOledSCL()
{
    return oledSCL;
}

byte MyHeatHardwareIO::getOledSDA()
{
    return oledSDA;
}

byte MyHeatHardwareIO::getEncA()
{
    return encA;
}

byte MyHeatHardwareIO::getEncB()
{
    return encB;
}

byte MyHeatHardwareIO::getEncBtn()
{
    return encBtn;
}

bool MyHeatHardwareIO::getIsReturnHomeOnSleep()
{
    return isReturnHomeOnSleep;
}

bool MyHeatHardwareIO::getEncInvert()
{
    return encInvert;
}

bool MyHeatHardwareIO::getIsEnabled()
{
    return isEnabled;
}

bool MyHeatHardwareIO::setIsEnabled(bool isEnabled)
{
    bool wasEnabled = this->isEnabled;
    this->isEnabled = isEnabled;

    if (isEnabled)
    {
        // Wake only when turning the display on, saving other settings keeps a sleeping screen asleep
        if (!wasEnabled)
        {
            isScreenOn = true;
            screenPowerSaveTimer = millis();
        }
        return true;
    }

    isScreenOn = false;
    if (u8g2)
    {
        u8g2->setPowerSave(true);
    }
    return false;
}

void MyHeatHardwareIO::manualDeserialize(const JsonDocument& data)
{
    deserialize(data);
    hardwareIOData->save();

    initOLED(oledSCL, oledSDA, oledAddress);
    initEncoder(encA, encB, encBtn, encInvert);

    reevaluateScreensCount();
}
