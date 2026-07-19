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
    bool isPreheating;
    SmokeSensorAlert smokeSensorAlert;
    MyHeatSave *smokeSensorData;
    uint32_t preheatTimer = 0;

    void serialize(JsonDocument &doc);
    void deserialize(const JsonDocument &doc);

public:
    MyHeatSmokeSensor();
    void begin();
    void resetPreheatTimer();
    void updateSmokeSensor();
    void manualDeserialize(const JsonDocument& payload);
    int getThreshold();
    bool getIsEnabled();
    bool getIsOverThreshold();
    SmokeSensorAlert getSmokeSensorAlert();
    void setSmokeSensorSettings(int threshold, int pin, bool isEnabled);
};

#endif
