#ifndef MYHEATTEMPERATURES_H
#define MYHEATTEMPERATURES_H

#include <Arduino.h>
#include "MyHeatSave.h"
#include <LittleFS.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <vector>
#include <array>

using DeviceAddressArray = std::array<uint8_t, 8>;

static constexpr uint8_t MAX_FAILED_READS = 2;
// Largest believable change between two reads (750 ms apart) in degrees C
static constexpr float MAX_TEMPERATURE_JUMP = 10.0f;
// How close a follow-up reading must be to confirm a suspicious one
static constexpr float CONFIRM_TOLERANCE = 1.0f;
// DS18B20 scratchpad value after power-on, before the first conversion finishes
static constexpr float POWER_ON_TEMPERATURE = 85.0f;

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
    std::vector<DeviceAddressArray> temperatureSensorsAddresses;
    std::vector<DeviceAddressArray> discoveredTemperatureSensorsAddresses;
    MyHeatSave *temperatureSensorData;

    std::vector<float> temperatures;
    // Consecutive unusable reads, the sensor is reported as failed once this reaches MAX_FAILED_READS
    std::vector<uint8_t> failedReads;
    // Suspicious reading (spike, power-on value, first value after a failure) waiting to be confirmed, NAN if none
    std::vector<float> candidateTemperatures;
    std::vector<TemperatureAlert> temperatureAlerts;
    byte temperaturePin;
    float minTemperature;
    float maxTemperature;

    OneWire oneWire;
    DallasTemperature temperatureSensors;

    void serialize(JsonDocument &doc);
    void deserialize(const JsonDocument &doc);
    void realocateMemory(byte newCount);
    void acceptTemperature(size_t index, float temperature);
    void registerFailedRead(size_t index);

public:
    MyHeatTemperatures();
    void begin();
    byte discoverTemperatureSensor();
    void setTemperatureSensorAddress(byte tempIndex, byte sensorAddressIndex);
    void deleteTemperatureSensorAddress(byte tempIndex);
    std::vector<DeviceAddressArray>& getDiscoveredTemperatureSensorAddresses();
    std::vector<DeviceAddressArray>& getTemperatureSensorAddresses();
    void updateTemperatures();
    std::vector<float>& getTemperatures();
    float getTemperature(byte index);
    void setTemperaturePin(byte newPin, bool isSave = true);
    void setTemperatureCount(byte newCount, bool isSave = true);
    void setTemperatureSettings(byte newPin, byte newCount, float minTemperature, float maxTemperature);
    byte getTemperatureCount();
    byte getTemperaturePin();
    float getMinTemperature();
    float getMaxTemperature();
    void manualDeserialize(const JsonDocument& payload);
    bool isTemperatureSensorAddressEmpty(byte index);
    void checkForAlerts();
    bool checkIsMinTemperature(byte index);
    bool checkIsMaxTemperature(byte index);
    bool checkIsBadConnection(byte index);
    TemperatureAlert getTemperatureAlert(byte index);
    void save();
};

#endif
