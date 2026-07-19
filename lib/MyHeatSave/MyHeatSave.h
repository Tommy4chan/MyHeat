#ifndef MYHEATSAVE_H
#define MYHEATSAVE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

class MyHeatSaveInterface
{
public:
    virtual void serialize(JsonDocument &json) = 0;
    virtual void deserialize(const JsonDocument &json) = 0;
    virtual void manualDeserialize(const JsonDocument& json) = 0;
};

class MyHeatSave
{
private:
    const char *path;
    MyHeatSaveInterface *data;

public:
    MyHeatSave(const char *path, MyHeatSaveInterface *data);
    void setPath(const char *path);
    void setData(MyHeatSaveInterface *data);
    bool save();
    bool read();
};

#endif
