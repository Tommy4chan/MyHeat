#include <Arduino.h>
#include <MyHeatTelebot.h>
#include <MyHeatDevice.h>
#include <LittleFS.h>
#include <MyHeatUtils.h>
#include <MyHeatWeb.h>
#include <MyHeatWifi.h>
#include <MyHeatHardwareIO.h>
#include "MyHeatAlerts.h"

MyHeatDevice& myHeatDevice = MyHeatDevice::getInstance();
MyHeatWifi& myHeatWifi = MyHeatWifi::getInstance();
MyHeatHardwareIO &hardwareIO = MyHeatHardwareIO::getInstance();

void setup()
{
	Serial.begin(115200);
	LittleFS.begin(true);
	
	myHeatWifi.begin();
	myHeatDevice.begin();
	hardwareIO.begin();
	MyHeatTelebot::begin();
	MyHeatWeb::begin();
}

void loop()
{
	if (myHeatWifi.isConnected())
	{
		MyHeatTelebot::tick();
	}

	myHeatWifi.tick();
	myHeatDevice.tick();
	hardwareIO.tick();
	MyHeatWeb::tick();
	MyHeatAlerts::tick();
}