#pragma once
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <MyHeatDevice.h>
#include <MyHeatWifi.h>

namespace MyHeatWeb
{
    inline AsyncWebServer server(80);
    inline MyHeatDevice *myHeatDevicePtr = nullptr;
    inline MyHeatWifi *myHeatWifiPtr = nullptr;

    void begin(MyHeatDevice *myHeatDevicePtr, MyHeatWifi *myHeatWifiPtr);
}