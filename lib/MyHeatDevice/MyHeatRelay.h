#pragma once
#include <Arduino.h>

class MyHeatRelay
{
private:
    byte mode;
    byte pin;
    bool isActive;

public:
    MyHeatRelay()
    {
        this->pin = 0;
        this->mode = 0;
        this->isActive = false;
    }

    void setPin(byte pin)
    {
        this->pin = pin;
        pinMode(this->pin, OUTPUT);
    }

    void changeMode()
    {        
        this->mode = (this->mode + 1) % 3;
    }

    void setIsActive(bool isActive)
    {
        this->isActive = isActive;
        digitalWrite(this->pin, this->isActive);
    }

    byte getMode()
    {
        return this->mode;
    }

    bool getIsActive()
    {
        return this->isActive;
    }
};