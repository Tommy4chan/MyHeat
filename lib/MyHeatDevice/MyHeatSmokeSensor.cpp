#include "MyHeatSmokeSensor.h"
#include <MyHeatAlerts.h>

void MyHeatSmokeSensor::serialize(JsonDocument &doc)
{
    doc[F("threshold")] = threshold;
    doc[F("pin")] = pin;
}

void MyHeatSmokeSensor::deserialize(JsonDocument &doc)
{
    threshold = doc[F("threshold")] | SMOKE_SENSOR_THRESHOLD;
    MyHeatAnalogSensor::begin(doc[F("pin")] | SMOKE_SENSOR_PIN);
}

MyHeatSmokeSensor::MyHeatSmokeSensor()
{
    pin = SMOKE_SENSOR_PIN;
    threshold = SMOKE_SENSOR_THRESHOLD;
    isEnabled = true;
    smokeSensorAlert = SmokeSensorAlert::SSA_NONE;
}

void MyHeatSmokeSensor::begin()
{
    smokeSensorData = new MyHeatSave("/smokeSensor.json", this);
    smokeSensorData->read();
    resetPreheatTimer();
}

void MyHeatSmokeSensor::resetPreheatTimer()
{
    preheatTimer = millis();
    isPreheating = true;
}

void MyHeatSmokeSensor::updateSmokeSensor()
{
    if (!isEnabled)
    {
        value = 0;
        return;
    }

    MyHeatAnalogSensor::read();

    SmokeSensorAlert newAlert = SmokeSensorAlert::SSA_NONE;

    if (value > threshold && !isPreheating)
    {
        newAlert = SmokeSensorAlert::SSA_OVER_THRESHOLD;
    }
    else if (value == 0)
    {
        newAlert = SmokeSensorAlert::SSA_BAD_CONNECTION;
        isPreheating = false;
    }

    if (newAlert != smokeSensorAlert)
    {
        smokeSensorAlert = newAlert;
        if (smokeSensorAlert == SmokeSensorAlert::SSA_OVER_THRESHOLD)
        {
            MyHeatAlerts::triggerAlert("Димовий датчик виявив дим");
        }
        else if (smokeSensorAlert == SmokeSensorAlert::SSA_BAD_CONNECTION)
        {
            MyHeatAlerts::triggerAlert("Димовий датчик не підключений");
            resetPreheatTimer();
        }
    }

    if (isPreheating && (millis() - preheatTimer) < SMOKE_SENSOR_PREHEAT_TIME && isEnabled)
    {
        value = ANALOG_INITIAL_VALUE;
    }
    else
    {
        isPreheating = false;
    }
}

void MyHeatSmokeSensor::manualDeserialize(JsonDocument payload)
{
    deserialize(payload);
    smokeSensorData->save();
}

int MyHeatSmokeSensor::getThreshold()
{
    return threshold;
}

bool MyHeatSmokeSensor::getIsEnabled()
{
    return isEnabled;
}

bool MyHeatSmokeSensor::getIsOverThreshold()
{
    return SmokeSensorAlert::SSA_OVER_THRESHOLD == smokeSensorAlert;
}

SmokeSensorAlert MyHeatSmokeSensor::getSmokeSensorAlert()
{
    return smokeSensorAlert;
}

void MyHeatSmokeSensor::setSmokeSensorSettings(int threshold, int pin, bool isEnabled)
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
