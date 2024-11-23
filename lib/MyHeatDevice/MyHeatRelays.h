#ifndef MYHEATRELAYS_H
#define MYHEATRELAYS_H

#include <Arduino.h>
#include "MyHeatRelay.h"
#include "MyHeatSave.h"
#include "MyHeatCustomFunction.h"
#include <LittleFS.h>

class MyHeatRelays : public MyHeatSaveInterface
{
private:
    MyHeatRelay relays[RELAY_COUNT];
    MyHeatSave *relaysData;
    MyHeatCustomFunction *customFunctions;

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
    void begin(MyHeatCustomFunction *customFunctions)
    {
        this->customFunctions = customFunctions;
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

    void updateRelays()
    {
        bool isSetRelayActive[RELAY_COUNT] = {false};

        for (int i = 0; i < FUNCTION_COUNT; i++)
        {
            if (customFunctions[i].getIsEnabled())
            {
                isSetRelayActive[customFunctions[i].getRelayIndex()] = isSetRelayActive[customFunctions[i].getRelayIndex()] || customFunctions[i].getIsActive();
            }
        }

        for (int i = 0; i < RELAY_COUNT; i++)
        {
            if (relays[i].getMode() == 0)
            {
                relays[i].setIsActive(false);
            }
            else if (relays[i].getMode() == 1)
            {
                relays[i].setIsActive(true);
            }
            else
            {
                relays[i].setIsActive(isSetRelayActive[i]);
            }
        }
    }
};
#endif
