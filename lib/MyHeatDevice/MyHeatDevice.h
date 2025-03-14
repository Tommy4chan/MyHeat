#ifndef MYHEATDEVICE_H
#define MYHEATDEVICE_H

#include <Arduino.h>
#include <LittleFS.h>
#include "MyHeatTemperatures.h"
#include "MyHeatCustomFunctions.h"
#include "MyHeatRelays.h"
#include "MyHeatUtils.h"
#include "MyHeatSmokeSensor.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

class MyHeatDevice : public MyHeatRelays, public MyHeatCustomFunctions, public MyHeatTemperatures, public MyHeatSmokeSensor
{
private:
    uint32_t tickTimerMain;
    uint32_t tickTimerSecondary;
    bool *isSetRelayActive;
    MyHeatDevice() {};

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
    void initIsSetRelayActive();
    void checkCustomFunctions();
    void updateRelays();
    void manualTick();
    void tick();
};

#endif