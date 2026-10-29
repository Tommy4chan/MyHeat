#ifndef MYHEATRELAY_H
#define MYHEATRELAY_H

#include <Arduino.h>
#include "MyHeatEnums.h"

class MyHeatRelay
{
private:
    RelayMode mode;
    byte pin;
    bool isActiveOnHigh;
    bool isActive;
    int8_t controllingFunction = -1; // Function that decided the state in auto mode, -1 if none

    void setPin(byte pin)
    {
        this->pin = pin;

        if (pin == 0) 
            return; // Pin 0 is not valid

        pinMode(pin, OUTPUT);
    }

    void setTrigger(bool isActiveOnHigh)
    {
        this->isActiveOnHigh = isActiveOnHigh;
    }

    void updateState() {
        if (pin == 0) 
            return; // Pin 0 is not valid
            
        digitalWrite(pin, isActive == isActiveOnHigh);
    }

public:
    MyHeatRelay()
    {
        pin = 0;
        mode = RelayMode::OFF;
        isActiveOnHigh = false;
        isActive = false;
    }

    void begin(byte pin, bool isActiveOnHigh)
    {
        setPin(pin);
        setTrigger(isActiveOnHigh);
        setIsActive(false);
        updateState();
    }

    void changeMode()
    {        
        mode = static_cast<RelayMode>((static_cast<byte>(mode) + 1) % 3);
        updateState();
    }

    void setMode(RelayMode mode)
    {
        this->mode = mode;
    }

    void setIsActive(bool isActive)
    {
        this->isActive = isActive;
        updateState();
    }

    RelayMode getMode()
    {
        return mode;
    }

    void setControllingFunction(int8_t functionIndex)
    {
        controllingFunction = functionIndex;
    }

    int8_t getControllingFunction()
    {
        return controllingFunction;
    }

    bool getIsActive()
    {
        return isActive;
    }

    byte getPin()
    {
        return pin;
    }

    bool getIsActiveOnHigh()
    {
        return isActiveOnHigh;
    }
};

#endif
