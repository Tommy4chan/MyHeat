#ifndef MYHEATRELAYS_H
#define MYHEATRELAYS_H

#include <Arduino.h>
#include "MyHeatRelay.h"
#include "MyHeatSave.h"
#include <LittleFS.h>

class MyHeatRelays : public MyHeatSaveInterface
{
private:
    MyHeatRelay *relays;
    MyHeatSave *relaysData;
    byte relayCount;

    void serialize(JsonDocument &doc);
    void deserialize(JsonDocument &doc);
    void realocateMemory(byte newCount);

public:
    MyHeatRelays();
    void begin();
    MyHeatRelay getRelay(byte relayIndex);
    MyHeatRelay *getRelays();
    void setRelayMode(byte relayIndex, RelayMode mode);
    void changeRelayMode(byte relayIndex);
    void setRelaySettings(byte relayIndex, byte newPin, bool isActiveOnHigh, bool isSave = true);
    void setRelayCount(byte newCount);
    byte getRelayCount();
    void manualDeserialize(JsonDocument payload);
    void save();
};

#endif
