#ifndef MYHEATCUSTOMFUNCTION_H
#define MYHEATCUSTOMFUNCTION_H

#include <Arduino.h>
#include "MyHeatEnums.h"

class MyHeatCustomFunction
{
private:
    CustomFunctionSign sign;
    byte temperatureIndex[2];
    float deltaValue[2];
    byte relayIndex;
    bool isEnabled;
    bool isActive;

public:
    MyHeatCustomFunction()
    {
        this->sign = CustomFunctionSign::LESS;
        this->temperatureIndex[0] = 0;
        this->temperatureIndex[1] = 0;
        this->deltaValue[0] = 0;
        this->deltaValue[1] = 0;
        this->relayIndex = 0;
        this->isEnabled = false;
        this->isActive = false;
    }

    void setSign(CustomFunctionSign sign)
    {
        this->sign = sign;
    }

    void setTemperatureIndex(byte tempIndex, byte tempSensorIndex)
    {
        this->temperatureIndex[tempIndex] = tempSensorIndex;
    }

    void setDeltaValue(byte deltaValueIndex, float deltaValue)
    {
        this->deltaValue[deltaValueIndex] = deltaValue;
    }

    void setRelayIndex(byte relayNumber)
    {
        this->relayIndex = relayNumber;
    }

    void setIsEnabled(bool isEnabled)
    {
        this->isEnabled = isEnabled;
    }

    void toggleIsEnabled()
    {
        this->isEnabled = !this->isEnabled;
    }

    bool isValid() const
    {
        return this->temperatureIndex[0] != TEMP_UNKNOWN && this->temperatureIndex[1] != TEMP_UNKNOWN && this->relayIndex != RELAY_UNKNOWN;
    }

    void setIsActive(bool isActive)
    {
        this->isActive = isActive;
    }

    CustomFunctionSign getSign() const
    {
        return this->sign;
    }

    byte getTemperatureIndex(byte tempIndex) const
    {
        return this->temperatureIndex[tempIndex];
    }

    float getDeltaValue(byte deltaValueIndex) const
    {
        return this->deltaValue[deltaValueIndex];
    }

    byte getRelayIndex() const
    {
        return this->relayIndex;
    }

    bool getIsEnabled() const
    {
        return this->isEnabled;
    }

    bool getIsActive() const
    {
        return this->isActive;
    }
};

#endif
