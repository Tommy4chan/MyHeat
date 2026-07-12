#include "MyHeatTemperatures.h"
#include <MyHeatAlerts.h>

void MyHeatTemperatures::serialize(JsonDocument &doc)
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

void MyHeatTemperatures::deserialize(JsonDocument &doc)
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

void MyHeatTemperatures::realocateMemory(byte newCount)
{
    if (newCount == temperatureCount)
    {
        return;
    }

    uint8_t **oldAddresses = nullptr;
    float *oldTemperatures = nullptr;

    if (temperatureCount > 0 && temperatureSensorsAddresses != nullptr && temperatures != nullptr)
    {
        oldAddresses = new uint8_t *[temperatureCount];
        oldTemperatures = new float[temperatureCount];
        for (byte i = 0; i < temperatureCount; i++)
        {
            oldAddresses[i] = new uint8_t[8];
            memcpy(oldAddresses[i], temperatureSensorsAddresses[i], 8);
            oldTemperatures[i] = temperatures[i];
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
    delete[] temperaturesFail;
    delete[] temperatureAlerts;

    temperatureSensorsAddresses = new uint8_t *[newCount];
    discoveredTemperatureSensorsAddresses = new uint8_t *[newCount];
    temperatures = new float[newCount];
    temperaturesFail = new bool[newCount];
    temperatureAlerts = new TemperatureAlert[newCount];

    for (byte i = 0; i < newCount; i++)
    {
        temperatureSensorsAddresses[i] = new uint8_t[8]{};
        discoveredTemperatureSensorsAddresses[i] = new uint8_t[8]{};
        temperatureAlerts[i] = TA_NONE;
        temperaturesFail[i] = false;

        if (oldAddresses != nullptr && i < temperatureCount)
        {
            memcpy(temperatureSensorsAddresses[i], oldAddresses[i], 8);
            temperatures[i] = oldTemperatures[i];
        }
        else
        {
            temperatures[i] = TEMPERATURE_ERROR;
        }
    }

    if (oldAddresses != nullptr)
    {
        for (byte i = 0; i < temperatureCount; i++)
        {
            delete[] oldAddresses[i];
        }
        delete[] oldAddresses;
        delete[] oldTemperatures;
    }

    temperatureCount = newCount;
}

MyHeatTemperatures::MyHeatTemperatures() : oneWire(), temperatureSensors()
{
    temperatureCount = 0;
    temperaturePin = TEMPERATURE_PIN;
    minTemperature = TEMPERATURE_MIN;
    maxTemperature = TEMPERATURE_MAX;
    temperatureSensorsAddresses = nullptr;
    discoveredTemperatureSensorsAddresses = nullptr;
    temperatures = nullptr;
    temperaturesFail = nullptr;
    temperatureAlerts = nullptr;
    realocateMemory(TEMPERATURE_COUNT);
}

void MyHeatTemperatures::begin()
{
    temperatureSensorData = new MyHeatSave("/temperatureSensors.json", this);
    temperatureSensorData->read();

    oneWire = OneWire(temperaturePin);
    temperatureSensors.setOneWire(&oneWire);
    temperatureSensors.setWaitForConversion(false);
    temperatureSensors.begin();
    temperatureSensors.setResolution(11);
    
    temperatureSensors.requestTemperatures();
    delay(1000);
}

byte MyHeatTemperatures::discoverTemperatureSensor()
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
            
        if (count >= temperatureCount)
            break;
            
        memcpy(discoveredTemperatureSensorsAddresses[count], address, 8);
        count++;
    }

    return count;
}

void MyHeatTemperatures::setTemperatureSensorAddress(byte tempIndex, byte sensorAddressIndex)
{
    memcpy(temperatureSensorsAddresses[tempIndex], discoveredTemperatureSensorsAddresses[sensorAddressIndex], 8);
    save();
}

void MyHeatTemperatures::deleteTemperatureSensorAddress(byte tempIndex)
{
    if (tempIndex >= temperatureCount)
        return;
        
    memset(temperatureSensorsAddresses[tempIndex], 0, 8);
    save();
}

uint8_t **MyHeatTemperatures::getDiscoveredTemperatureSensorAddresses()
{
    return discoveredTemperatureSensorsAddresses;
}

uint8_t **MyHeatTemperatures::getTemperatureSensorAddresses()
{
    return temperatureSensorsAddresses;
}

void MyHeatTemperatures::updateTemperatures()
{
    for (byte i = 0; i < temperatureCount; i++)
    {
        if (isTemperatureSensorAddressEmpty(i))
        {
            temperatures[i] = TEMPERATURE_ERROR;
            continue;
        }

        float newTemperature = temperatureSensors.getTempC(temperatureSensorsAddresses[i]);

        if (temperaturesFail[i]) {
            temperatures[i] = newTemperature;

            if (newTemperature != TEMPERATURE_ERROR)
            {
                temperaturesFail[i] = false;
            }
        }
        else {
            if (newTemperature == TEMPERATURE_ERROR)
            {
                temperaturesFail[i] = true;
            }
            else {
                temperatures[i] = newTemperature;
            }
        }
    }
    temperatureSensors.requestTemperatures();
    checkForAlerts();
}

float *MyHeatTemperatures::getTemperatures()
{
    return temperatures;
}

float MyHeatTemperatures::getTemperature(byte index)
{
    if (index == TN_INDEX)
        return 0;

    return temperatures[index];
}

void MyHeatTemperatures::setTemperaturePin(byte newPin, bool isSave)
{
    temperaturePin = newPin;
    oneWire = OneWire(temperaturePin);
    temperatureSensors.setOneWire(&oneWire);
    temperatureSensors.setWaitForConversion(false);
    temperatureSensors.setResolution(11);
    temperatureSensors.begin();

    if (isSave)
    {
        save();
    }
}

void MyHeatTemperatures::setTemperatureCount(byte newCount, bool isSave)
{
    realocateMemory(newCount);

    if (isSave)
    {
        save();
    }
}

void MyHeatTemperatures::setTemperatureSettings(byte newPin, byte newCount, float minTemperature, float maxTemperature)
{
    setTemperaturePin(newPin, false);
    setTemperatureCount(newCount, false);
    this->minTemperature = minTemperature;
    this->maxTemperature = maxTemperature;
    save();
}

byte MyHeatTemperatures::getTemperatureCount()
{
    return temperatureCount;
}

byte MyHeatTemperatures::getTemperaturePin()
{
    return temperaturePin;
}

float MyHeatTemperatures::getMinTemperature()
{
    return minTemperature;
}

float MyHeatTemperatures::getMaxTemperature()
{
    return maxTemperature;
}

void MyHeatTemperatures::manualDeserialize(JsonDocument payload)
{
    deserialize(payload);
    save();
}

bool MyHeatTemperatures::isTemperatureSensorAddressEmpty(byte index)
{
    return temperatureSensorsAddresses[index][0] == 0;
}

void MyHeatTemperatures::checkForAlerts() {
    for (byte i = 0; i < temperatureCount; i++)
    {
        if (isTemperatureSensorAddressEmpty(i))
        {
            continue;
        }

        TemperatureAlert newAlert = TA_NONE;
        if (checkIsBadConnection(i))
        {
            newAlert = TA_BAD_CONNECTION;
        }
        else if (checkIsMinTemperature(i))
        {
            newAlert = TA_MIN;
        }
        else if (checkIsMaxTemperature(i))
        {
            newAlert = TA_MAX;
        }

        if (newAlert != temperatureAlerts[i])
        {
            temperatureAlerts[i] = newAlert;
            if (newAlert != TA_NONE)
            {
                String message = "";
                if (newAlert == TA_MIN)
                {
                    message = "Датчик температури Т" + String(i) + " має значення нижче мінімальної температури";
                }
                else if (newAlert == TA_MAX)
                {
                    message = "Датчик температури Т" + String(i) + " має значення вище максимальної температури";
                }
                else if (newAlert == TA_BAD_CONNECTION)
                {
                    message = "Датчик температури Т" + String(i) + " відключився";
                }

                MyHeatAlerts::triggerAlert(message);
            }
        }
    }
}

bool MyHeatTemperatures::checkIsMinTemperature(byte index)
{
    return temperatures[index] < minTemperature;
}

bool MyHeatTemperatures::checkIsMaxTemperature(byte index)
{
    return temperatures[index] > maxTemperature;
}

bool MyHeatTemperatures::checkIsBadConnection(byte index)
{
    return temperatures[index] <= TEMPERATURE_ERROR + 0.1;
}

TemperatureAlert MyHeatTemperatures::getTemperatureAlert(byte index)
{
    return temperatureAlerts[index];
}

void MyHeatTemperatures::save()
{
    temperatureSensorData->save();
}
