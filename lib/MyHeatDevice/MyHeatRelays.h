#ifndef MYHEATRELAYS_H
#define MYHEATRELAYS_H

#include <Arduino.h>
#include "MyHeatRelay.h"
#include "MyHeatSave.h"
#include <LittleFS.h>
#include <vector>

class MyHeatRelays : public MyHeatSaveInterface
{
private:
    std::vector<MyHeatRelay> relays;
    MyHeatSave *relaysData;

    void serialize(JsonDocument &doc);
    void deserialize(const JsonDocument &doc);
    void realocateMemory(byte newCount);

public:
    MyHeatRelays();
    void begin();
    MyHeatRelay getRelay(byte relayIndex);
    std::vector<MyHeatRelay>& getRelays();
    void setRelayMode(byte relayIndex, RelayMode mode);
    void changeRelayMode(byte relayIndex);
    void setRelaySettings(byte relayIndex, byte newPin, bool isActiveOnHigh, bool isSave = true);
    void setRelayCount(byte newCount);
    byte getRelayCount();
    void manualDeserialize(const JsonDocument& payload);
    void save();
};

#endif
