#ifndef MYHEATSAVE_H
#define MYHEATSAVE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FS.h>

class MyHeatSaveInterface
{
public:
    virtual void serialize(JsonDocument &json) = 0;
    virtual void deserialize(JsonDocument &json) = 0;
};

class MyHeatSave
{
private:
    fs::FS *_fs;
    const char *_path;
    MyHeatSaveInterface *_data;
    uint16_t _tout;

public:
    MyHeatSave(fs::FS *nfs, const char *path, MyHeatSaveInterface *data, uint16_t tout = 5000)
    {
        setFS(nfs, path);
        setData(data);
        setTimeout(tout);
    }

    void setFS(fs::FS *nfs, const char *path)
    {
        _fs = nfs;
        _path = path;
    }

    void setData(MyHeatSaveInterface *data)
    {
        _data = data;
    }

    void setTimeout(uint16_t tout)
    {
        _tout = tout;
    }

    bool save()
    {
        File file = _fs->open(_path, FILE_WRITE);
        if (!file)
        {
            Serial.println("Failed to open file for writing");
            return false;
        }

        JsonDocument doc;
        _data->serialize(doc);

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
        File file = _fs->open(_path, FILE_READ);
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

        _data->deserialize(doc);

        file.close();
        return true;
    }
};
#endif
