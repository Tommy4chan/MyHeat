#include "MyHeatUtils.h"

namespace MyHeatUtils
{
    String getConvertedStateToText(byte state)
    {
        String text = "";

        if (state == 0)
            text = "Вимкнено";
        else if (state == 1)
            text = "Увімкнено";
        else
            text = "Авто";

        return text;
    }

    String getConvertedActiveToText(bool active)
    {
        return active ? "Так" : "Ні";
    }

    String getCurrentDateAndTime()
    {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo, 100))
        {
            Serial.println("Failed to obtain time");
            return F("Помилка отримання часу\n\n");
        }

        char buffer[20];
        strftime(buffer, sizeof(buffer), "%d.%m.%Y %H:%M:%S", &timeinfo);
        return String(buffer) + "\n\n";
    }

    // Set from the web server task, read from the main loop
    static volatile uint32_t lastFlashUpdateActivity = 0;
    static volatile bool isFlashUpdateActive = false;
    // An upload that stops sending (closed tab, lost WiFi) stops counting as in progress after this
    static constexpr uint32_t FLASH_UPDATE_TIMEOUT = 10000;

    void markFlashUpdateActivity()
    {
        lastFlashUpdateActivity = millis();
        isFlashUpdateActive = true;
    }

    void clearFlashUpdateActivity()
    {
        isFlashUpdateActive = false;
    }

    bool isFlashUpdateInProgress()
    {
        return isFlashUpdateActive && millis() - lastFlashUpdateActivity < FLASH_UPDATE_TIMEOUT;
    }

    const char *getResetReasonName()
    {
        switch (esp_reset_reason())
        {
        case ESP_RST_POWERON:   return "power_on";
        case ESP_RST_SW:        return "restart";
        case ESP_RST_PANIC:     return "crash";
        case ESP_RST_INT_WDT:
        case ESP_RST_TASK_WDT:
        case ESP_RST_WDT:       return "watchdog";
        case ESP_RST_BROWNOUT:  return "brownout";
        case ESP_RST_EXT:       return "reset_pin";
        case ESP_RST_DEEPSLEEP: return "deep_sleep";
        default:                return "unknown";
        }
    }

    String getResetReasonText()
    {
        switch (esp_reset_reason())
        {
        case ESP_RST_POWERON:   return F("подано живлення");
        case ESP_RST_SW:        return F("перезапуск");
        case ESP_RST_PANIC:     return F("збій програми");
        case ESP_RST_INT_WDT:
        case ESP_RST_TASK_WDT:
        case ESP_RST_WDT:       return F("зависання (watchdog)");
        case ESP_RST_BROWNOUT:  return F("просадка живлення");
        case ESP_RST_EXT:       return F("кнопка скидання");
        case ESP_RST_DEEPSLEEP: return F("вихід зі сну");
        default:                return F("невідома");
        }
    }

    String getAddressToString(uint8_t address[8])
    {
        String text = "";
        for (int i = 0; i < 8; i++)
        {
            text += String(address[i], HEX);
            if (i < 7)
                text += ":";
        }

        return text;
    }

    JsonDocument getFileContent(String path)
    {
        JsonDocument doc;
        File file = LittleFS.open(path, FILE_READ);

        if (!file)
        {
            Serial.println("Failed to open file for reading");
            return doc;
        }

        DeserializationError error = deserializeJson(doc, file);

        if (error)
        {
            Serial.println("Failed to read file, using default configuration");
            file.close();
            return doc;
        }

        file.close();
        return doc;
    }

    bool isFloat(const String &str)
    {
        const char *cstr = str.c_str();
        char *end;
        double floatValue = strtod(cstr, &end);

        return *end == '\0';
    }
}