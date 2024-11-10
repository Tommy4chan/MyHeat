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

    bool isFloat(const String &str)
    {
        const char *cstr = str.c_str();
        char *end;
        double floatValue = strtod(cstr, &end);

        return *end == '\0';
    }

    bool isTimeDefault()
    {
        struct tm timeinfo;
        return !getLocalTime(&timeinfo, 100);
    }
}