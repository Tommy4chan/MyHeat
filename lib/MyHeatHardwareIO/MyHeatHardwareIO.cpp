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
}

void MyHeatHardwareIO::showTemperature()
{
    byte cursor = 24;
    byte temperatureIndexStart = 3 * menuIndex;

    MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
    byte temperatureIndexEnd = min((byte)(temperatureIndexStart + 3), myHeatDevice.temperatures.getTemperatureCount());

    for (byte i = temperatureIndexStart; i < temperatureIndexEnd; i++)
    {
        float temperature = myHeatDevice.temperatures.getTemperature(i);

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

void MyHeatHardwareIO::showRelay()
{
    byte relayIndex = menuIndex - maxTemperatureScreens;
    MyHeatRelay relay = MyHeatDevice::getInstance().relays.getRelay(relayIndex);
    u8g2->print(F("Реле "));
    u8g2->print(String(relayIndex) + ":");
    u8g2->setCursor(0, 40);
    u8g2->print("Стан: " + MyHeatUtils::getConvertedStateToText(static_cast<byte>(relay.getMode())));
    u8g2->setCursor(0, 56);
    u8g2->print("Активне: " + MyHeatUtils::getConvertedActiveToText(relay.getIsActive()));
}

void MyHeatHardwareIO::showSmokeSensor()
{
    MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
    u8g2->print(F("Датчик диму: "));
    u8g2->setCursor(0, 40);
    u8g2->print(myHeatDevice.smokeSensor.getValue() == -1 ? "Ініціалізація" : String(myHeatDevice.smokeSensor.getValue()));
    u8g2->setCursor(0, 56);
    u8g2->print("Небезпечно: " + MyHeatUtils::getConvertedActiveToText(myHeatDevice.smokeSensor.getIsOverThreshold()));
}

void MyHeatHardwareIO::handleEncoder()
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
    else if (eb->hold(1))
    {
        MyHeatWifi::getInstance().switchWifiMode();
        showWifiSwitchModal();
    }
    else if (!isModalActive && eb->holdFor(5000))
    {
        showQRCodeModal();
    }
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
    u8g2->setCursor(0, 24);

    if (menuIndex < maxTemperatureScreens)
    {
        u8g2->setFont(u8g2_font_10x20_t_cyrillic);
        showTemperature();
    }
    else if (menuIndex >= maxTemperatureScreens && menuIndex < maxTemperatureScreens + maxRelayScreens)
    {
        u8g2->setFont(u8g2_font_8x13_t_cyrillic);
        showRelay();
    }
    else if (menuIndex >= maxTemperatureScreens + maxRelayScreens && menuIndex < maxScreens)
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
        u8g2->setPowerSave(true);
    }

    if (isScreenOn && !isModalActive && millis() - screenUpdateTimer >= 1000)
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

    maxTemperatureScreens = ceil((float)myHeatDevice.temperatures.getTemperatureCount() / 3);
    maxRelayScreens = myHeatDevice.relays.getRelayCount();
    maxScreens = maxTemperatureScreens + maxRelayScreens + 1;
}

void MyHeatHardwareIO::setSettings(byte type, byte address, byte scl, byte sda, int powerSaveInterval, byte a, byte b, byte btn, bool invert, bool isEnabled)
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
    this->isEnabled = isEnabled;

    if (isEnabled)
    {
        isScreenOn = true;
        screenPowerSaveTimer = millis();
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
