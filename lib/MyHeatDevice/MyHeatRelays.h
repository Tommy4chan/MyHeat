#ifndef MYHEATRELAYS_H
#define MYHEATRELAYS_H

#include <Arduino.h>
#include "MyHeatRelay.h"
#include "MyHeatSave.h"
#include <LittleFS.h>

class MyHeatRelays : public MyHeatSaveInterface
{
private:
    MyHeatRelay relays[RELAY_COUNT];
    MyHeatSave *relaysData;

    void initRelays()
    {
        byte relayPinsArray[] = RELAY_PINS;
        bool relayTriggerArray[] = RELAY_TRIGGER;

        for (int i = 0; i < RELAY_COUNT; i++)
        {
            relays[i].begin(relayPinsArray[i], relayTriggerArray[i]);
        }
    }

    void serialize(JsonDocument &doc)
    {
        for (int i = 0; i < RELAY_COUNT; i++)
        {
            doc[F("relays")][i][F("mode")] = relays[i].getMode();
        }
    }

    void deserialize(JsonDocument &doc)
    {
        for (int i = 0; i < RELAY_COUNT; i++)
        {
            relays[i].setMode(doc[F("relays")][i][F("mode")]);
        }
    }

public:
    void begin()
    {
        relaysData = new MyHeatSave(&LittleFS, "/relays.json", this);
        relaysData->read();
        initRelays();
    }

    MyHeatRelay getRelay(byte relayIndex)
    {
        return relays[relayIndex];
    }

    MyHeatRelay *getRelays()
    {
        return relays;
    }

    void changeRelayMode(byte relayIndex)
    {
        relays[relayIndex].changeMode();
        relaysData->save();
    }
};
#endif
