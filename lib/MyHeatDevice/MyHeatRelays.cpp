#include "MyHeatRelays.h"

void MyHeatRelays::serialize(JsonDocument &doc)
{
    doc[F("relayCount")] = relayCount;

    for (int i = 0; i < relayCount; i++)
    {
        doc[F("relays")][i][F("mode")] = static_cast<byte>(relays[i].getMode());
        doc[F("relays")][i][F("pin")] = relays[i].getPin();
        doc[F("relays")][i][F("isActiveOnHigh")] = relays[i].getIsActiveOnHigh();
    }
}

void MyHeatRelays::deserialize(JsonDocument &doc)
{
    realocateMemory(doc[F("relayCount")] | RELAY_COUNT);

    for (int i = 0; i < relayCount; i++)
    {
        relays[i].setMode(static_cast<RelayMode>(doc[F("relays")][i][F("mode")] | 0));
        relays[i].begin(doc[F("relays")][i][F("pin")] | 2, doc[F("relays")][i][F("isActiveOnHigh")] | false);
    }
}

void MyHeatRelays::realocateMemory(byte newCount)
{
    MyHeatRelay *oldRelays = nullptr;
    if (relays != nullptr)
    {
        oldRelays = new MyHeatRelay[relayCount];
        for (byte i = 0; i < relayCount; i++)
        {
            oldRelays[i] = relays[i];
        }
    }

    delete[] relays;

    relays = new MyHeatRelay[newCount];
    if (oldRelays != nullptr)
    {
        byte copyCount = min(relayCount, newCount);
        for (byte i = 0; i < copyCount; i++)
        {
            relays[i] = oldRelays[i];
        }

        delete[] oldRelays;
    }

    relayCount = newCount;
}

MyHeatRelays::MyHeatRelays()
{
    relayCount = 0;
    relays = nullptr;
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

MyHeatRelay *MyHeatRelays::getRelays()
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
    if (relayIndex >= relayCount)
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
    return relayCount;
}

void MyHeatRelays::manualDeserialize(JsonDocument payload)
{
    deserialize(payload);
    save();
}

void MyHeatRelays::save()
{
    relaysData->save();
}
