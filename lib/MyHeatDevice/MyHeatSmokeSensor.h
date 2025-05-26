#ifndef MYHEATSMOKESENSOR_H
#define MYHEATSMOKESENSOR_H

#include <Arduino.h>
#include "MyHeatAnalogSensor.h"
#include "MyHeatSave.h"

enum SmokeSensorAlert
{
    SSA_NONE = 0,
    SSA_OVER_THRESHOLD = 1,
    SSA_BAD_CONNECTION = 2
};

class MyHeatSmokeSensor : public MyHeatAnalogSensor, public MyHeatSaveInterface
{
private:
    int threshold;
    bool isEnabled;
    bool isNotified;
    bool isPreheating = true;
    SmokeSensorAlert smokeSensorAlert;
    MyHeatSave *smokeSensorData;
    uint32_t preheatTimer = 0;

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
        isEnabled = true;
        isNotified = false;
        smokeSensorAlert = SmokeSensorAlert::SSA_NONE;
    }

    void begin()
    {
        smokeSensorData = new MyHeatSave("/smokeSensor.json", this);
        smokeSensorData->read();
        resetPreheatTimer();
    }

    void resetPreheatTimer()
    {
        preheatTimer = millis();
        isPreheating = true;
    }

    void updateSmokeSensor()
    {
        if (!isEnabled)
        {
            value = 0;
            return;
        }

        MyHeatAnalogSensor::read();

        if (value > threshold && !isPreheating)
        {
            smokeSensorAlert = SmokeSensorAlert::SSA_OVER_THRESHOLD;
        }
        else if (value == 0)
        {
            smokeSensorAlert = SmokeSensorAlert::SSA_BAD_CONNECTION;
            isPreheating = false;
        }
        else
        {
            if (smokeSensorAlert == SmokeSensorAlert::SSA_BAD_CONNECTION)
            {
                resetPreheatTimer();
            }

            smokeSensorAlert = SmokeSensorAlert::SSA_NONE;
        }

        if (smokeSensorAlert == SmokeSensorAlert::SSA_NONE && isNotified)
        {
            isNotified = false;
        }

        if (isPreheating && millis() < (preheatTimer + SMOKE_SENSOR_PREHEAT_TIME) && isEnabled)
        {
            value = ANALOG_INITIAL_VALUE;
        }
        else
        {
            isPreheating = false;
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
        return SmokeSensorAlert::SSA_OVER_THRESHOLD == smokeSensorAlert;
    }

    SmokeSensorAlert getSmokeSensorAlert(bool forceNotification)
    {
        if (smokeSensorAlert != SmokeSensorAlert::SSA_NONE && (!isNotified || forceNotification))
        {
            isNotified = true;
            return smokeSensorAlert;
        }

        return SmokeSensorAlert::SSA_NONE;
    }

    void setSmokeSensorSettings(int threshold, int pin, bool isEnabled)
    {
        this->threshold = threshold;
        this->isEnabled = isEnabled;

        if (isEnabled)
        {
            resetPreheatTimer();
        }

        MyHeatAnalogSensor::begin(pin);
        smokeSensorData->save();
    }
};

#endif
