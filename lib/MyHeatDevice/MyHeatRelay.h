#pragma once
#include <Arduino.h>

class MyHeatRelay
{
private:
    byte mode;
    byte pin;
    bool trigger;
    bool isActive;

public:
    MyHeatRelay()
    {
        this->pin = 0;
        this->mode = 0;
        this->trigger = false;
        this->isActive = false;
    }

    void setPin(byte pin)
    {
        this->pin = pin;
        pinMode(this->pin, OUTPUT);
        update();
    }

    void setTrigger(bool trigger)
    {
        this->trigger = trigger;
    }

    void changeMode()
    {        
        this->mode = (this->mode + 1) % 3;
    }

    void setIsActive(bool isActive)
    {
        this->isActive = isActive;
        update();
    }

    void update() {
        digitalWrite(this->pin, this->isActive ? trigger : !trigger);
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