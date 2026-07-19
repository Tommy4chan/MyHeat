#include <Arduino.h>
#include <MyHeatTelebot.h>
#include <MyHeatDevice.h>
#include <LittleFS.h>
#include <MyHeatUtils.h>
#include <MyHeatWeb.h>
#include <MyHeatWifi.h>
#include <MyHeatHardwareIO.h>
#include "MyHeatAlerts.h"
#include "MyHeatMqtt.h"

MyHeatDevice& myHeatDevice = MyHeatDevice::getInstance();
MyHeatWifi& myHeatWifi = MyHeatWifi::getInstance();
MyHeatHardwareIO &hardwareIO = MyHeatHardwareIO::getInstance();

bool isSystemLoaded = false;

void setup()
{
	Serial.begin(115200);
	LittleFS.begin(true);
	
	myHeatWifi.begin();
	myHeatDevice.begin();
	hardwareIO.begin();
	hardwareIO.startLoadingScreen();
	MyHeatTelebot::begin();
	MyHeatWeb::begin();
	MyHeatMqtt::begin();
}

void loop()
{
    if (myHeatWifi.isConnected())
    {
        if (!isSystemLoaded && !MyHeatUtils::isTimeDefault()) {
            MyHeatTelebot::tick();
            isSystemLoaded = true;
            hardwareIO.stopLoadingScreen();
        } else if (isSystemLoaded) {
            MyHeatTelebot::tick();
        }
    }
    
    if (!isSystemLoaded && (myHeatWifi.isAPMode() || millis() > SYSTEM_LOAD_TIMEOUT)) {
        isSystemLoaded = true;
        hardwareIO.stopLoadingScreen();
    }

	myHeatWifi.tick();
	myHeatDevice.tick();
    
    if (isSystemLoaded) {
	    hardwareIO.tick();
    }
	MyHeatWeb::tick();
	MyHeatMqtt::tick();
}