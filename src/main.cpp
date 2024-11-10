#include <Arduino.h>
#include <MyHeatTelebot.h>
#include <MyHeatDevice.h>
#include <LittleFS.h>
#include <MyHeatUtils.h>
#include <MyHeatWeb.h>
#include <MyHeatWifi.h>

MyHeatDevice& myHeatDevice = MyHeatDevice::getInstance();
MyHeatWifi& myHeatWifi = MyHeatWifi::getInstance();

void setup()
{
	Serial.begin(115200);
	LittleFS.begin(true);
	
	myHeatWifi.begin();
	myHeatDevice.begin();
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
}