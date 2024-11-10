#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include "MyHeatTemperatures.h"
#include "MyHeatCustomFunctions.h"
#include "MyHeatRelays.h"
#include "MyHeatUtils.h"
#include "MyHeatHardwareIO.h"

#define XSTR(x) #x
#define STR(x) XSTR(x)

class MyHeatDevice : public MyHeatRelays, public MyHeatCustomFunctions, public MyHeatTemperatures
{
private:
    MyHeatHardwareIO hardwareIO;
    uint32_t tickTimerMain;
    uint32_t tickTimerSecondary;
    MyHeatDevice() {};

public:
    static MyHeatDevice& getInstance() {
        static MyHeatDevice instance;
        return instance;
    }

    MyHeatDevice(const MyHeatDevice&) = delete;
    MyHeatDevice& operator=(const MyHeatDevice&) = delete;

    void begin();
    void tick();
};