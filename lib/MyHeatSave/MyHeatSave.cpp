#include "MyHeatSave.h"

MyHeatSave::MyHeatSave(const char *path, MyHeatSaveInterface *data)
{
    setPath(path);
    setData(data);
}

void MyHeatSave::setPath(const char *path)
{
    this->path = path;
}

void MyHeatSave::setData(MyHeatSaveInterface *data)
{
    this->data = data;
}

bool MyHeatSave::save()
{
    File file = LittleFS.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println(String("Failed to open file for writing: ") + path);
        return false;
    }

    JsonDocument doc;
    data->serialize(doc);

    if (serializeJson(doc, file) == 0)
    {
        Serial.println(String("Failed to write to file: ") + path);
        file.close();
        return false;
    }

    file.close();
    return true;
}

bool MyHeatSave::read()
{
    if(!LittleFS.exists(path))
    {
        save();
        return false;
    }

    File file = LittleFS.open(path, FILE_READ);
    if (!file)
    {
        Serial.println(String("Failed to open file for reading: ") + path);
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.println(String("Failed to read file, using default configuration: ") + path);
        file.close();
        return false;
    }

    data->deserialize(doc);

    file.close();
    return true;
}
