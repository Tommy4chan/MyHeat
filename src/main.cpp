#include <Arduino.h>
#include <MyHeatTelebot.h>
#include <MyHeatDevice.h>
#include <LittleFS.h>

#define XSTR(x) #x
#define STR(x) XSTR(x)

MyHeatDevice myHeatDevice;

void setup()
{
	Serial.begin(115200);

	WiFi.begin(STR(WIFI_SSID), STR(WIFI_PASSWORD));
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	Serial.println("Connected");

	configTime(NTP_OFFSET, NTP_DAYLIGHT_OFFSET, STR(NTP_SERVER));

	LittleFS.begin();

	myHeatDevice.begin();
	MyHeatTelebot::begin(STR(TELEGRAM_BOT_TOKEN), &myHeatDevice);
}

void loop()
{
	MyHeatTelebot::tick();
	myHeatDevice.tick();
}