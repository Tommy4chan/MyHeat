#ifndef MYHEATDEVICE_H
#define MYHEATDEVICE_H

#include <Arduino.h>
#include <LittleFS.h>
#include "MyHeatTemperatures.h"
#include "MyHeatCustomFunctions.h"
#include "MyHeatRelays.h"
#include "MyHeatUtils.h"
#include "MyHeatSmokeSensor.h"

#define XSTR(...) #__VA_ARGS__
#define STR(x) XSTR(x)

class MyHeatDevice
{
private:
    uint32_t tickTimerMain;
    uint32_t tickTimerSecondary;
    MyHeatDevice() {
        tickTimerMain = 0;
        tickTimerSecondary = 0;
    };

public:
    MyHeatRelays relays;
    MyHeatCustomFunctions customFunctions;
    MyHeatTemperatures temperatures;
    MyHeatSmokeSensor smokeSensor;

public:
    static MyHeatDevice &getInstance()
    {
        static MyHeatDevice instance;
        return instance;
    }

    MyHeatDevice(const MyHeatDevice &) = delete;
    MyHeatDevice &operator=(const MyHeatDevice &) = delete;

    void begin();
    void validateCustomFunctions();
    void checkCustomFunctions();
    void updateRelays();
    void manualTick();
    void tick();

    // Bumped every time device state is refreshed or recalculated. Consumers (web, MQTT) compare it
    // to skip all work while nothing could have changed, then hash payloads to find what actually did
    uint32_t getStateRevision() { return stateRevision; }

private:
    uint32_t stateRevision = 0;
};

#endif