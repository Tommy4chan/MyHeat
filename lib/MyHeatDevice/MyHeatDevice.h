#pragma once
#include <Arduino.h>
#include <FileData.h>
#include <LittleFS.h>
#include "MyHeatCustomFunction.h"
#include "MyHeatRelay.h"
#include "MyHeatUtils.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define XSTR(x) #x
#define STR(x) XSTR(x)

// #include <FS.h>

// Створити все в цьому неймспейсі
// Використовувати цю лібу https://github.com/GyverLibs/GyverDS18 і цей клас GyverDS18

class MyHeatDevice
{
private:
    MyHeatCustomFunction customFunctions[FUNCTION_COUNT];
    MyHeatRelay relays[RELAY_COUNT];
    uint8_t temperatureSensorsAddresses[TEMPERATURE_COUNT][8];
    uint8_t discoveredTemperatureSensorsAddresses[TEMPERATURE_COUNT][8];
    FileData *customFunctionsData;
    FileData *relaysData;
    FileData *temperatureSensorData;

    float temperatures[TEMPERATURE_COUNT];
    uint32_t tickTimer;
    OneWire oneWire;
    DallasTemperature temperatureSensors;

    void readFileData(FileData *fileData);
    void checkCustomFunctions();
    void updateRelays();
    void updateTemperature();

public:
    MyHeatDevice() : oneWire(TEMPERATURE_PIN), temperatureSensors(&oneWire) {}
    void begin();
    void initRelays();
    MyHeatCustomFunction getCustomFunction(byte index);
    MyHeatCustomFunction *getCustomFunctions();
    void setCustomFunctionIsEnabled(byte functionIndex);
    void setCustomFunctionSign(byte functionIndex, byte sign);
    void setCustomFunctionTemperatureIndex(byte functionIndex, byte tempIndex, byte tempSensorIndex);
    void setCustomFunctionDeltaValue(byte functionIndex, byte deltaValueIndex, float deltaValue);
    void setCustomFunctionRelayIndex(byte functionIndex, byte relayIndex);
    byte discoverTemperatureSensor();
    uint8_t (*getDiscoveredTemperatureSensorAddresses())[8];
    uint8_t (*getTemperatureSensorAddresses())[8];
    void setTemperatureSensorAddress(byte tempIndex, byte sensorAddressIndex);
    void deleteTemperatureSensorAddress(byte tempIndex);
    
    MyHeatRelay getRelay(byte relayIndex);
    MyHeatRelay *getRelays();
    void changeRelayMode(byte relayIndex);

    float* getTemperatures();

    void tick();
};