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
    uint8_t **temperatureSensorsAddresses;
    uint8_t **discoveredTemperatureSensorsAddresses;
    MyHeatSave *temperatureSensorData;

    float *temperatures;
    byte temperatureCount;
    byte temperaturePin;
    OneWire oneWire;
    DallasTemperature temperatureSensors;

    void serialize(JsonDocument &doc)
    {
        doc[F("temperaturePin")] = temperaturePin;
        doc[F("temperatureCount")] = temperatureCount;

        for (int i = 0; i < temperatureCount; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                doc[F("temperatureSensorsAddresses")][i][j] = temperatureSensorsAddresses[i][j];
            }
        }
    }

    void deserialize(JsonDocument &doc)
    {
        temperaturePin = doc[F("temperaturePin")];
        realocateMemory(doc[F("temperatureCount")]);

        for (int i = 0; i < temperatureCount; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                temperatureSensorsAddresses[i][j] = doc[F("temperatureSensorsAddresses")][i][j];
            }
        }
    }

    void realocateMemory(byte newCount)
    {
        if (newCount == temperatureCount)
        {
            return;
        }

        uint8_t **oldAddresses = nullptr;
        if (temperatureCount > 0 && temperatureSensorsAddresses != nullptr)
        {
            oldAddresses = new uint8_t *[temperatureCount];
            for (byte i = 0; i < temperatureCount; i++)
            {
                oldAddresses[i] = new uint8_t[8];
                memcpy(oldAddresses[i], temperatureSensorsAddresses[i], 8);
            }
        }

        if (temperatureSensorsAddresses != nullptr)
        {
            for (byte i = 0; i < temperatureCount; i++)
            {
                delete[] temperatureSensorsAddresses[i];
                delete[] discoveredTemperatureSensorsAddresses[i];
            }
            delete[] temperatureSensorsAddresses;
            delete[] discoveredTemperatureSensorsAddresses;
        }
        delete[] temperatures;

        temperatureSensorsAddresses = new uint8_t *[newCount];
        discoveredTemperatureSensorsAddresses = new uint8_t *[newCount];
        temperatures = new float[newCount];

        for (byte i = 0; i < newCount; i++)
        {
            temperatureSensorsAddresses[i] = new uint8_t[8]{};
            discoveredTemperatureSensorsAddresses[i] = new uint8_t[8]{};
            temperatures[i] = -127.00;

            if (oldAddresses != nullptr && i < temperatureCount)
            {
                memcpy(temperatureSensorsAddresses[i], oldAddresses[i], 8);
            }
        }

        if (oldAddresses != nullptr)
        {
            for (byte i = 0; i < temperatureCount; i++)
            {
                delete[] oldAddresses[i];
            }
            delete[] oldAddresses;
        }

        temperatureCount = newCount;
    }

public:
    MyHeatTemperatures() : oneWire(), temperatureSensors()
    {
        temperatureCount = 0;
        temperaturePin = 23;
        temperatureSensorsAddresses = nullptr;
        discoveredTemperatureSensorsAddresses = nullptr;
        temperatures = nullptr;
    };

    void begin()
    {
        temperatureSensorData = new MyHeatSave(&LittleFS, "/temperatureSensors.json", this);
        temperatureSensorData->read();

        oneWire = OneWire(temperaturePin);
        temperatureSensors.setOneWire(&oneWire);
        temperatureSensors.setWaitForConversion(false);
        temperatureSensors.begin();

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
            for (int i = 0; i < temperatureCount; i++)
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
        save();
    }

    void deleteTemperatureSensorAddress(byte tempIndex)
    {
        memset(temperatureSensorsAddresses[tempIndex], 0, 8);
        save();
    }

    uint8_t **getDiscoveredTemperatureSensorAddresses()
    {
        return discoveredTemperatureSensorsAddresses;
    }

    uint8_t **getTemperatureSensorAddresses()
    {
        return temperatureSensorsAddresses;
    }

    void updateTemperatures()
    {
        for (byte i = 0; i < temperatureCount; i++)
        {
            if (temperatureSensorsAddresses[i][0] == 0)
            {
                temperatures[i] = -127.00;
                continue;
            }

            temperatures[i] = temperatureSensors.getTempC(temperatureSensorsAddresses[i]);
        }
        temperatureSensors.requestTemperatures();
    }

    float *getTemperatures()
    {
        return temperatures;
    }

    float getTemperature(byte index)
    {
        if (index == TN_INDEX)
            return 0;

        return temperatures[index];
    }

    void setTemperaturePin(byte newPin)
    {
        temperaturePin = newPin;
        oneWire = OneWire(temperaturePin);
        temperatureSensors.setOneWire(&oneWire);
        temperatureSensors.setWaitForConversion(false);
        temperatureSensors.begin();
    }

    void setTemperatureCount(byte newCount)
    {
        realocateMemory(newCount);
    }

    byte getTemperatureCount()
    {
        return temperatureCount;
    }

    byte getTemperaturePin()
    {
        return temperaturePin;
    }

    void save()
    {
        temperatureSensorData->save();
    }
};
#endif
