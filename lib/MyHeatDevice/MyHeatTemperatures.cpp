#include "MyHeatTemperatures.h"
#include <MyHeatAlerts.h>

void MyHeatTemperatures::serialize(JsonDocument &doc)
{
    doc[F("temperaturePin")] = temperaturePin;
    doc[F("minTemperature")] = minTemperature;
    doc[F("maxTemperature")] = maxTemperature;
    doc[F("temperatureCount")] = temperatureSensorsAddresses.size();

    for (size_t i = 0; i < temperatureSensorsAddresses.size(); i++)
    {
        for (int j = 0; j < 8; j++)
        {
            doc[F("temperatureSensorsAddresses")][i][j] = temperatureSensorsAddresses[i][j];
        }
    }
}

void MyHeatTemperatures::deserialize(const JsonDocument &doc)
{
    temperaturePin = doc[F("temperaturePin")] | TEMPERATURE_PIN;
    minTemperature = doc[F("minTemperature")] | TEMPERATURE_MIN;
    maxTemperature = doc[F("maxTemperature")] | TEMPERATURE_MAX;
    realocateMemory(doc[F("temperatureCount")] | TEMPERATURE_COUNT);

    for (size_t i = 0; i < temperatureSensorsAddresses.size(); i++)
    {
        for (int j = 0; j < 8; j++)
        {
            temperatureSensorsAddresses[i][j] = doc[F("temperatureSensorsAddresses")][i][j];
        }
    }
}

void MyHeatTemperatures::realocateMemory(byte newCount)
{
    size_t oldCount = temperatures.size();
    temperatureSensorsAddresses.resize(newCount);
    discoveredTemperatureSensorsAddresses.resize(newCount);
    temperatures.resize(newCount, TEMPERATURE_ERROR);
    temperaturesFail.resize(newCount, false);
    temperatureAlerts.resize(newCount, TA_NONE);
    
    // For newly allocated elements, make sure to zero their arrays
    for (size_t i = oldCount; i < newCount; i++) {
        temperatureSensorsAddresses[i].fill(0);
        discoveredTemperatureSensorsAddresses[i].fill(0);
    }
}

MyHeatTemperatures::MyHeatTemperatures() : oneWire(), temperatureSensors()
{
    temperaturePin = TEMPERATURE_PIN;
    minTemperature = TEMPERATURE_MIN;
    maxTemperature = TEMPERATURE_MAX;
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
        for (size_t i = 0; i < temperatureSensorsAddresses.size(); i++)
        {
            if (memcmp(temperatureSensorsAddresses[i].data(), address, 8) == 0)
            {
                flag = true;
                break;
            }
        }

        if (flag)
            continue;
            
        if (count >= discoveredTemperatureSensorsAddresses.size())
            break;
            
        memcpy(discoveredTemperatureSensorsAddresses[count].data(), address, 8);
        count++;
    }

    return count;
}

void MyHeatTemperatures::setTemperatureSensorAddress(byte tempIndex, byte sensorAddressIndex)
{
    memcpy(temperatureSensorsAddresses[tempIndex].data(), discoveredTemperatureSensorsAddresses[sensorAddressIndex].data(), 8);
    save();
}

void MyHeatTemperatures::deleteTemperatureSensorAddress(byte tempIndex)
{
    if (tempIndex >= temperatureSensorsAddresses.size())
        return;
        
    memset(temperatureSensorsAddresses[tempIndex].data(), 0, 8);
    save();
}

std::vector<DeviceAddressArray>& MyHeatTemperatures::getDiscoveredTemperatureSensorAddresses()
{
    return discoveredTemperatureSensorsAddresses;
}

std::vector<DeviceAddressArray>& MyHeatTemperatures::getTemperatureSensorAddresses()
{
    return temperatureSensorsAddresses;
}

void MyHeatTemperatures::updateTemperatures()
{
    for (size_t i = 0; i < temperatures.size(); i++)
    {
        if (isTemperatureSensorAddressEmpty(i))
        {
            temperatures[i] = TEMPERATURE_ERROR;
            continue;
        }

        float newTemperature = temperatureSensors.getTempC(temperatureSensorsAddresses[i].data());

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

std::vector<float>& MyHeatTemperatures::getTemperatures()
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
    return temperatures.size();
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

void MyHeatTemperatures::manualDeserialize(const JsonDocument& payload)
{
    deserialize(payload);
    save();
}

bool MyHeatTemperatures::isTemperatureSensorAddressEmpty(byte index)
{
    return temperatureSensorsAddresses[index][0] == 0;
}

void MyHeatTemperatures::checkForAlerts() {
    for (size_t i = 0; i < temperatures.size(); i++)
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
