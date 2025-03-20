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
    byte temperatureCount;
    byte temperaturePin;
    float minTemperature;
    float maxTemperature;
    TemperatureAlert *temperatureAlerts;
    bool *isNotified;

    OneWire oneWire;
    DallasTemperature temperatureSensors;

    void serialize(JsonDocument &doc)
    {
        doc[F("temperaturePin")] = temperaturePin;
        doc[F("minTemperature")] = minTemperature;
        doc[F("maxTemperature")] = maxTemperature;
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
        temperaturePin = doc[F("temperaturePin")] | TEMPERATURE_PIN;
        minTemperature = doc[F("minTemperature")] | TEMPERATURE_MIN;
        maxTemperature = doc[F("maxTemperature")] | TEMPERATURE_MAX;
        realocateMemory(doc[F("temperatureCount")] | TEMPERATURE_COUNT);

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
        temperatureAlerts = new TemperatureAlert[newCount];
        isNotified = new bool[newCount];

        for (byte i = 0; i < newCount; i++)
        {
            temperatureSensorsAddresses[i] = new uint8_t[8]{};
            discoveredTemperatureSensorsAddresses[i] = new uint8_t[8]{};
            temperatures[i] = TEMPERATURE_ERROR;
            temperatureAlerts[i] = TA_NONE;
            isNotified[i] = false;

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
        temperaturePin = TEMPERATURE_PIN;
        minTemperature = TEMPERATURE_MIN;
        maxTemperature = TEMPERATURE_MAX;
        temperatureSensorsAddresses = nullptr;
        discoveredTemperatureSensorsAddresses = nullptr;
        temperatures = nullptr;
        temperatureAlerts = nullptr;
        isNotified = nullptr;
        realocateMemory(TEMPERATURE_COUNT);
    };

    void begin()
    {
        temperatureSensorData = new MyHeatSave("/temperatureSensors.json", this);
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
        if (tempIndex >= temperatureCount)
            return;
            
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
            if (isTemperatureSensorAddressEmpty(i))
            {
                temperatures[i] = TEMPERATURE_ERROR;
                continue;
            }

            temperatures[i] = temperatureSensors.getTempC(temperatureSensorsAddresses[i]);
        }
        temperatureSensors.requestTemperatures();
        checkForAlerts();
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

    void setTemperaturePin(byte newPin, bool isSave = true)
    {
        temperaturePin = newPin;
        oneWire = OneWire(temperaturePin);
        temperatureSensors.setOneWire(&oneWire);
        temperatureSensors.setWaitForConversion(false);
        temperatureSensors.begin();

        if (isSave)
        {
            save();
        }
    }

    void setTemperatureCount(byte newCount, bool isSave = true)
    {
        realocateMemory(newCount);

        if (isSave)
        {
            save();
        }
    }

    void setTemperatureSettings(byte newPin, byte newCount, float minTemperature, float maxTemperature)
    {
        setTemperaturePin(newPin, false);
        setTemperatureCount(newCount, false);
        this->minTemperature = minTemperature;
        this->maxTemperature = maxTemperature;
        save();
    }

    byte getTemperatureCount()
    {
        return temperatureCount;
    }

    byte getTemperaturePin()
    {
        return temperaturePin;
    }

    float getMinTemperature()
    {
        return minTemperature;
    }

    float getMaxTemperature()
    {
        return maxTemperature;
    }

    void manualDeserialize(JsonDocument data)
    {
        deserialize(data);
        save();
    }

    bool isTemperatureSensorAddressEmpty(byte index)
    {
        return temperatureSensorsAddresses[index][0] == 0;
    }

    void checkForAlerts() {
        for (byte i = 0; i < temperatureCount; i++)
        {
            if (isTemperatureSensorAddressEmpty(i))
            {
                continue;
            }

            if (checkIsBadConnection(i))
            {
                temperatureAlerts[i] = TA_BAD_CONNECTION;
            }
            else if (checkIsMinTemperature(i))
            {
                temperatureAlerts[i] = TA_MIN;
            }
            else if (checkIsMaxTemperature(i))
            {
                temperatureAlerts[i] = TA_MAX;
            }
            else
            {
                temperatureAlerts[i] = TA_NONE;
            }

            if (temperatureAlerts[i] == TA_NONE && isNotified[i])
            {
                isNotified[i] = false;
            }
        }
    }

    bool checkIsMinTemperature(byte index)
    {
        return temperatures[index] < minTemperature;
    }

    bool checkIsMaxTemperature(byte index)
    {
        return temperatures[index] > maxTemperature;
    }

    bool checkIsBadConnection(byte index)
    {
        return temperatures[index] == TEMPERATURE_ERROR;
    }

    TemperatureAlert getTemperatureAlert(byte index)
    {
        if (temperatureAlerts[index] != TA_NONE && !isNotified[index])
        {
            isNotified[index] = true;
            return temperatureAlerts[index];
        }

        return TA_NONE;
    }

    void save()
    {
        temperatureSensorData->save();
    }
};

#endif
