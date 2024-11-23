#ifndef MYHEATWEB_H
#define MYHEATWEB_H

#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <MyHeatDevice.h>
#include <MyHeatWifi.h>

namespace MyHeatWeb
{
    inline AsyncWebServer server(80);
    inline MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
    inline MyHeatWifi &myHeatWifi = MyHeatWifi::getInstance();

    void begin();
}
#endif
