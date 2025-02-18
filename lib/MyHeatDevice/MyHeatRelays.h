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

    void serialize(JsonDocument &doc)
    {
        doc[F("relayCount")] = relayCount;

        for (int i = 0; i < relayCount; i++)
        {
            doc[F("relays")][i][F("mode")] = relays[i].getMode();
            doc[F("relays")][i][F("pin")] = relays[i].getPin();
            doc[F("relays")][i][F("isActiveOnHigh")] = relays[i].getIsActiveOnHigh();
        }
    }

    void deserialize(JsonDocument &doc)
    {
        realocateMemory(doc[F("relayCount")]);

        for (int i = 0; i < relayCount; i++)
        {
            relays[i].setMode(doc[F("relays")][i][F("mode")]);
            relays[i].begin(doc[F("relays")][i][F("pin")], doc[F("relays")][i][F("isActiveOnHigh")]);
        }
    }

    void realocateMemory(byte newCount)
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

public:
    MyHeatRelays()
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

    void begin()
    {
        relaysData = new MyHeatSave("/relays.json", this);
        relaysData->read();
    }

    MyHeatRelay getRelay(byte relayIndex)
    {
        return relays[relayIndex];
    }

    MyHeatRelay *getRelays()
    {
        return relays;
    }

    void setRelayMode(byte relayIndex, byte mode)
    {
        relays[relayIndex].setMode(mode);
        save();
    }

    void changeRelayMode(byte relayIndex)
    {
        relays[relayIndex].changeMode();
        save();
    }

    void setRelaySettings(byte relayIndex, byte newPin, bool isActiveOnHigh, bool isSave = true)
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

    void setRelayCount(byte newCount)
    {
        realocateMemory(newCount);
        save();
    }

    byte getRelayCount()
    {
        return relayCount;
    }

    void save()
    {
        relaysData->save();
    }
};

#endif
