#include "MyHeatWebSettings.h"

namespace MyHeatWeb
{
    JsonDocument getUsedPinsData()
    {
        JsonDocument usedPinsData;
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        usedPinsData[F("usedPins")][0] = myHeatDevice.getTemperaturePin();
        usedPinsData[F("usedPins")][1] = ENC_A;
        usedPinsData[F("usedPins")][2] = ENC_B;
        usedPinsData[F("usedPins")][3] = ENC_BTN;
        usedPinsData[F("usedPins")][4] = OLED_SCL;
        usedPinsData[F("usedPins")][5] = OLED_SDA;

        for (int i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            usedPinsData[F("usedPins")][i + 6] = myHeatDevice.getRelay(i).getPin();
        }

        return usedPinsData;
    }
}