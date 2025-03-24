#ifndef MYHEATSMOKESENSOR_H
#define MYHEATSMOKESENSOR_H

#include <Arduino.h>
#include "MyHeatAnalogSensor.h"
#include "MyHeatSave.h"

class MyHeatSmokeSensor : public MyHeatAnalogSensor, public MyHeatSaveInterface
{
private:
    int threshold;
    bool isOverThreshold;
    bool isEnabled;
    bool isNotified;
    MyHeatSave *smokeSensorData;

    void serialize(JsonDocument &doc)
    {
        doc[F("threshold")] = threshold;
        doc[F("pin")] = pin;
    }

    void deserialize(JsonDocument &doc)
    {
        threshold = doc[F("threshold")] | SMOKE_SENSOR_THRESHOLD;
        MyHeatAnalogSensor::begin(doc[F("pin")] | SMOKE_SENSOR_PIN);
    }

public:
    MyHeatSmokeSensor()
    {
        pin = SMOKE_SENSOR_PIN;
        threshold = SMOKE_SENSOR_THRESHOLD;
        isOverThreshold = false;
        isEnabled = true;
        isNotified = false;
    }

    void begin()
    {
        smokeSensorData = new MyHeatSave("/smokeSensor.json", this);
        smokeSensorData->read();
    }

    void updateSmokeSensor()
    {
        if (millis() < 25000) {
            return;
        }

        if(!isEnabled)
        {
            value = 0;
            return;
        }

        MyHeatAnalogSensor::read();
        
        isOverThreshold = value > threshold;

        if(!isOverThreshold && isNotified)
        {
            isNotified = false;
        }
    }

    void manualDeserialize(JsonDocument payload)
    {
        deserialize(payload);
        smokeSensorData->save();
    }

    int getThreshold()
    {
        return threshold;
    }

    bool getIsEnabled()
    {
        return isEnabled;
    }

    bool getIsOverThreshold()
    {
        return isOverThreshold;
    }

    bool getIsSendSmokeSensorNotification()
    {
        if (isOverThreshold && !isNotified)
        {
            isNotified = true;
            return true;
        }

        return false;
    }

    void setSmokeSensorSettings(int threshold, int pin, bool isEnabled)
    {
        this->threshold = threshold;
        this->isEnabled = isEnabled;
        MyHeatAnalogSensor::begin(pin);
        smokeSensorData->save();
    }
};

#endif
