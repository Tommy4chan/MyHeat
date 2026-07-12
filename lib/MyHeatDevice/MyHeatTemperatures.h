#ifndef MYHEATTEMPERATURES_H
#define MYHEATTEMPERATURES_H

#include <Arduino.h>
#include "MyHeatSave.h"
#include <LittleFS.h>
#include <OneWire.h>
#include <DallasTemperature.h>

enum TemperatureAlert
{
    TA_NONE = 0,
    TA_MIN = 1,
    TA_MAX = 2,
    TA_BAD_CONNECTION = 3
};

class MyHeatTemperatures : public MyHeatSaveInterface
{
private:
    uint8_t **temperatureSensorsAddresses;
    uint8_t **discoveredTemperatureSensorsAddresses;
    MyHeatSave *temperatureSensorData;

    float *temperatures;
    bool *temperaturesFail;
    byte temperatureCount;
    byte temperaturePin;
    float minTemperature;
    float maxTemperature;
    TemperatureAlert *temperatureAlerts;

    OneWire oneWire;
    DallasTemperature temperatureSensors;

    void serialize(JsonDocument &doc);
    void deserialize(JsonDocument &doc);
    void realocateMemory(byte newCount);

public:
    MyHeatTemperatures();
    void begin();
    byte discoverTemperatureSensor();
    void setTemperatureSensorAddress(byte tempIndex, byte sensorAddressIndex);
    void deleteTemperatureSensorAddress(byte tempIndex);
    uint8_t **getDiscoveredTemperatureSensorAddresses();
    uint8_t **getTemperatureSensorAddresses();
    void updateTemperatures();
    float *getTemperatures();
    float getTemperature(byte index);
    void setTemperaturePin(byte newPin, bool isSave = true);
    void setTemperatureCount(byte newCount, bool isSave = true);
    void setTemperatureSettings(byte newPin, byte newCount, float minTemperature, float maxTemperature);
    byte getTemperatureCount();
    byte getTemperaturePin();
    float getMinTemperature();
    float getMaxTemperature();
    void manualDeserialize(JsonDocument payload);
    bool isTemperatureSensorAddressEmpty(byte index);
    void checkForAlerts();
    bool checkIsMinTemperature(byte index);
    bool checkIsMaxTemperature(byte index);
    bool checkIsBadConnection(byte index);
    TemperatureAlert getTemperatureAlert(byte index);
    void save();
};

#endif
