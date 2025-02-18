#ifndef MYHEATSAVE_H
#define MYHEATSAVE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

class MyHeatSaveInterface
{
public:
    virtual void serialize(JsonDocument &json) = 0;
    virtual void deserialize(JsonDocument &json) = 0;
};

class MyHeatSave
{
private:
    const char *path;
    MyHeatSaveInterface *data;
    uint16_t tout;

public:
    MyHeatSave(const char *path, MyHeatSaveInterface *data, uint16_t tout = 5000)
    {
        setPath(path);
        setData(data);
        setTimeout(tout);
    }

    void setPath(const char *path)
    {
        this->path = path;
    }

    void setData(MyHeatSaveInterface *data)
    {
        this->data = data;
    }

    void setTimeout(uint16_t tout)
    {
        this->tout = tout;
    }

    bool save()
    {
        File file = LittleFS.open(path, FILE_WRITE);
        if (!file)
        {
            Serial.println("Failed to open file for writing");
            return false;
        }

        JsonDocument doc;
        data->serialize(doc);

        if (serializeJson(doc, file) == 0)
        {
            Serial.println("Failed to write to file");
            file.close();
            return false;
        }

        file.close();
        return true;
    }

    bool read()
    {
        File file = LittleFS.open(path, FILE_READ);
        if (!file)
        {
            Serial.println("Failed to open file for reading");
            return false;
        }

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, file);
        if (error)
        {
            Serial.println("Failed to read file, using default configuration");
            file.close();
            return false;
        }

        data->deserialize(doc);

        file.close();
        return true;
    }
};

#endif
