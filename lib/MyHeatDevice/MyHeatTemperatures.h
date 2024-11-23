#ifndef MYHEATTEMPERATURES_H
#define MYHEATTEMPERATURES_H

#include <Arduino.h>
#include "MyHeatSave.h"
#include <LittleFS.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class MyHeatTemperatures : public MyHeatSaveInterface
{
private:
    uint8_t temperatureSensorsAddresses[TEMPERATURE_COUNT][8];
    uint8_t discoveredTemperatureSensorsAddresses[TEMPERATURE_COUNT][8];
    MyHeatSave *temperatureSensorData;

    float temperatures[TEMPERATURE_COUNT];
    OneWire oneWire;
    DallasTemperature temperatureSensors;

    void serialize(JsonDocument &doc)
    {
        for (int i = 0; i < TEMPERATURE_COUNT; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                doc[F("temperatureSensorsAddresses")][i][j] = temperatureSensorsAddresses[i][j];
            }
        }
    }

    void deserialize(JsonDocument &doc)
    {
        for (int i = 0; i < TEMPERATURE_COUNT; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                temperatureSensorsAddresses[i][j] = doc[F("temperatureSensorsAddresses")][i][j];
            }
        }
    }

public:
    MyHeatTemperatures() : oneWire(TEMPERATURE_PIN), temperatureSensors(&oneWire) {};

    void begin()
    {
        temperatureSensors.begin();
        temperatureSensors.setWaitForConversion(false);
        temperatureSensorData = new MyHeatSave(&LittleFS, "/temperatureSensors.json", this);
        temperatureSensorData->read();
        temperatureSensors.requestTemperatures();
        delay(1000);
    }

    byte discoverTemperatureSensor()
    {
        uint8_t address[8];
        byte count = 0;

        while (oneWire.search(address))
        {
            bool flag = false;
            for (int i = 0; i < TEMPERATURE_COUNT; i++)
            {
                if (memcmp(temperatureSensorsAddresses[i], address, 8) == 0)
                {
                    flag = true;
                    break;
                }
            }

            if (flag)
                continue;
            memcpy(discoveredTemperatureSensorsAddresses[count], address, 8);
            count++;
        }

        return count;
    }

    void setTemperatureSensorAddress(byte tempIndex, byte sensorAddressIndex)
    {
        memcpy(temperatureSensorsAddresses[tempIndex], discoveredTemperatureSensorsAddresses[sensorAddressIndex], 8);
        temperatureSensorData->save();
    }

    void deleteTemperatureSensorAddress(byte tempIndex)
    {
        memset(temperatureSensorsAddresses[tempIndex], 0, 8);
        temperatureSensorData->save();
    }

    uint8_t (*getDiscoveredTemperatureSensorAddresses())[8]
    {
        return discoveredTemperatureSensorsAddresses;
    }

    uint8_t (*getTemperatureSensorAddresses())[8]
    {
        return temperatureSensorsAddresses;
    }

    void updateTemperature()
    {
        for (byte i = 0; i < TEMPERATURE_COUNT; i++)
        {
            temperatures[i] = temperatureSensors.getTempC(temperatureSensorsAddresses[i]);
        }
        temperatureSensors.requestTemperatures();
    }

    float *getTemperatures()
    {
        return temperatures;
    }
};
#endif
