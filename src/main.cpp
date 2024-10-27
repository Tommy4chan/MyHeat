#include <Arduino.h>
#include <MyHeatTelebot.h>
#include <MyHeatDevice.h>
#include <LittleFS.h>

#define XSTR(x) #x
#define STR(x) XSTR(x)

MyHeatDevice myHeatDevice;

unsigned long wifiReconnectTick = 0;

void setup()
{
	Serial.begin(115200);

	WiFi.begin(STR(WIFI_SSID), STR(WIFI_PASSWORD));
	delay(2000);

	configTime(NTP_OFFSET, NTP_DAYLIGHT_OFFSET, STR(NTP_SERVER));

	LittleFS.begin();

	myHeatDevice.begin();
	MyHeatTelebot::begin(STR(TELEGRAM_BOT_TOKEN), &myHeatDevice);
}

void loop()
{
	if ((WiFi.status() != WL_CONNECTED) && (millis() - wifiReconnectTick >= 30000))
	{
		Serial.println("Reconnecting to WiFi...");
		WiFi.disconnect();
		WiFi.reconnect();
		wifiReconnectTick = millis();
	}
	
	if (WiFi.status() == WL_CONNECTED) {
		MyHeatTelebot::tick();
	}
	
	myHeatDevice.tick();
}