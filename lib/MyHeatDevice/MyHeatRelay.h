#pragma once
#include <Arduino.h>

class MyHeatRelay
{
private:
    byte mode;
    byte pin;
    bool isTriggerOnHigh;
    bool isActive;

    void setPin(byte pin)
    {
        this->pin = pin;
        pinMode(pin, OUTPUT);
        updateState();
    }

    void setTrigger(bool isTriggerOnHigh)
    {
        this->isTriggerOnHigh = isTriggerOnHigh;
    }

    void updateState() {
        digitalWrite(pin, isActive == isTriggerOnHigh);
    }

public:
    MyHeatRelay()
    {
        pin = 0;
        mode = 0;
        isTriggerOnHigh = false;
        isActive = false;
    }

    void begin(byte pin, bool isTriggerOnHigh)
    {
        setPin(pin);
        setTrigger(isTriggerOnHigh);
        setIsActive(false);
        updateState();
    }

    void changeMode()
    {        
        mode = (mode + 1) % 3;
    }

    void setIsActive(bool isActive)
    {
        this->isActive = isActive;
        updateState();
    }

    byte getMode()
    {
        return mode;
    }

    bool getIsActive()
    {
        return isActive;
    }
};