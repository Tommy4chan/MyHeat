#include "MyHeatRelays.h"

void MyHeatRelays::serialize(JsonDocument &doc)
{
    doc[F("relayCount")] = relays.size();

    for (size_t i = 0; i < relays.size(); i++)
    {
        doc[F("relays")][i][F("mode")] = static_cast<byte>(relays[i].getMode());
        doc[F("relays")][i][F("pin")] = relays[i].getPin();
        doc[F("relays")][i][F("isActiveOnHigh")] = relays[i].getIsActiveOnHigh();
    }
}

void MyHeatRelays::deserialize(const JsonDocument &doc)
{
    realocateMemory(doc[F("relayCount")] | RELAY_COUNT);

    for (size_t i = 0; i < relays.size(); i++)
    {
        relays[i].setMode(static_cast<RelayMode>(doc[F("relays")][i][F("mode")] | 0));
        relays[i].begin(doc[F("relays")][i][F("pin")] | 2, doc[F("relays")][i][F("isActiveOnHigh")] | false);
    }
}

void MyHeatRelays::realocateMemory(byte newCount)
{
    relays.resize(newCount);
}

MyHeatRelays::MyHeatRelays()
{
    realocateMemory(RELAY_COUNT);

    byte relayPinsArray[] = RELAY_PINS;
    bool relayTriggerArray[] = RELAY_TRIGGER;

    for (int i = 0; i < RELAY_COUNT; i++)
    {
        relays[i].begin(relayPinsArray[i], relayTriggerArray[i]);
    }
}

void MyHeatRelays::begin()
{
    relaysData = new MyHeatSave("/relays.json", this);
    relaysData->read();
}

MyHeatRelay MyHeatRelays::getRelay(byte relayIndex)
{
    return relays[relayIndex];
}

std::vector<MyHeatRelay>& MyHeatRelays::getRelays()
{
    return relays;
}

void MyHeatRelays::setRelayMode(byte relayIndex, RelayMode mode)
{
    relays[relayIndex].setMode(mode);
    save();
}

void MyHeatRelays::changeRelayMode(byte relayIndex)
{
    relays[relayIndex].changeMode();
    save();
}

void MyHeatRelays::setRelaySettings(byte relayIndex, byte newPin, bool isActiveOnHigh, bool isSave)
{
    if (relayIndex >= relays.size())
    {
        return;
    }
    
    relays[relayIndex].begin(newPin, isActiveOnHigh);

    if (isSave)
    {
        save();
    }
}

void MyHeatRelays::setRelayCount(byte newCount)
{
    realocateMemory(newCount);
    save();
}

byte MyHeatRelays::getRelayCount()
{
    return relays.size();
}

void MyHeatRelays::manualDeserialize(const JsonDocument& payload)
{
    deserialize(payload);
    save();
}

void MyHeatRelays::save()
{
    relaysData->save();
}
