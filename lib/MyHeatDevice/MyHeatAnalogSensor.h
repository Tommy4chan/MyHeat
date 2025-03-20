#ifndef MYHEATANALOGSENSOR_H
#define MYHEATANALOGSENSOR_H

#include <Arduino.h>

class MyHeatAnalogSensor
{
protected:
    byte pin;
    int value;

    void setPin(byte pin)
    {
        this->pin = pin;
        pinMode(pin, INPUT);
    }

public:
    MyHeatAnalogSensor()
    {
        pin = 0;
        value = ANALOG_INITIAL_VALUE;
    }

    void begin(byte pin)
    {
        setPin(pin);
    }

    byte getPin()
    {
        return pin;
    }

    void read()
    {
        value = analogRead(pin);
    }

    int getValue()
    {
        return value;
    }
};

#endif
