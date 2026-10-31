#include "MyHeatTemperatures.h"
#include <MyHeatAlerts.h>
#include <MyHeatUtils.h>

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
    failedReads.resize(newCount, 0);
    candidateTemperatures.resize(newCount, NAN);
    isStateKnown.resize(newCount, false);
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
    delay(temperatureSensors.millisToWaitForConversion(11));
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
    // OTA flash writes corrupt OneWire reads and would raise false disconnect alerts, the last values are kept instead
    if (MyHeatUtils::isFlashUpdateInProgress())
    {
        return;
    }

    for (size_t i = 0; i < temperatures.size(); i++)
    {
        if (isTemperatureSensorAddressEmpty(i))
        {
            temperatures[i] = TEMPERATURE_ERROR;
            continue;
        }

        float reading = temperatureSensors.getTempC(temperatureSensorsAddresses[i].data());

        if (reading == DEVICE_DISCONNECTED_C)
        {
            registerFailedRead(i);
            continue;
        }

        // A DS18B20 cannot physically change by more than a few degrees between two reads. Bigger jumps come from
        // corrupted reads on a bouncing connector (the 8-bit CRC passes ~1 in 256 garbage frames) or from the
        // power-on scratchpad value, so they are only trusted once the next reading agrees
        // The very first reading after boot has nothing to compare with, it is trusted unless it is the power-on value
        bool isFirstReading = !isStateKnown[i];
        bool isCurrentValid = temperatures[i] != TEMPERATURE_ERROR;
        bool isPlausible = reading != POWER_ON_TEMPERATURE &&
                           (isFirstReading || (isCurrentValid && fabsf(reading - temperatures[i]) <= MAX_TEMPERATURE_JUMP));

        if (isPlausible)
        {
            acceptTemperature(i, reading);
        }
        else if (!isnan(candidateTemperatures[i]) && fabsf(reading - candidateTemperatures[i]) <= CONFIRM_TOLERANCE)
        {
            acceptTemperature(i, reading);
        }
        else
        {
            candidateTemperatures[i] = reading;
            registerFailedRead(i);
        }
    }
    temperatureSensors.requestTemperatures();
    checkForAlerts();
}

void MyHeatTemperatures::acceptTemperature(size_t index, float temperature)
{
    temperatures[index] = temperature;
    failedReads[index] = 0;
    candidateTemperatures[index] = NAN;
    isStateKnown[index] = true;
}

void MyHeatTemperatures::registerFailedRead(size_t index)
{
    // A single bad read keeps the last good value, repeated ones mark the sensor as failed
    if (failedReads[index] < MAX_FAILED_READS)
    {
        failedReads[index]++;
    }

    if (failedReads[index] >= MAX_FAILED_READS)
    {
        temperatures[index] = TEMPERATURE_ERROR;
        isStateKnown[index] = true;
    }
}

std::vector<float>& MyHeatTemperatures::getTemperatures()
{
    return temperatures;
}

float MyHeatTemperatures::getTemperature(byte index)
{
    if (index >= temperatures.size())
        return TEMPERATURE_ERROR;

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

static AlertType toAlertType(TemperatureAlert alert)
{
    switch (alert)
    {
    case TA_MIN:
        return AlertType::TEMPERATURE_LOW;
    case TA_MAX:
        return AlertType::TEMPERATURE_HIGH;
    default:
        return AlertType::TEMPERATURE_DISCONNECTED;
    }
}

static String getTemperatureAlertText(TemperatureAlert alert, size_t index, bool isResolved)
{
    String sensor = "Датчик температури Т" + String(index);
    switch (alert)
    {
    case TA_MIN:
        return sensor + (isResolved ? " повернувся вище мінімальної температури" : " має значення нижче мінімальної температури");
    case TA_MAX:
        return sensor + (isResolved ? " повернувся нижче максимальної температури" : " має значення вище максимальної температури");
    default:
        return sensor + (isResolved ? " знову підключений" : " відключився");
    }
}

void MyHeatTemperatures::checkForAlerts() {
    for (size_t i = 0; i < temperatures.size(); i++)
    {
        // Before the first reading the initial -127 would look like a disconnected sensor
        if (isTemperatureSensorAddressEmpty(i) || !isStateKnown[i])
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
            TemperatureAlert oldAlert = temperatureAlerts[i];
            temperatureAlerts[i] = newAlert;

            if (oldAlert != TA_NONE)
            {
                MyHeatAlerts::resolveAlert(toAlertType(oldAlert), i, getTemperatureAlertText(oldAlert, i, true));
            }
            if (newAlert != TA_NONE)
            {
                MyHeatAlerts::triggerAlert(toAlertType(newAlert), i, getTemperatureAlertText(newAlert, i, false));
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
