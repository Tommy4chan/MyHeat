#include <Arduino.h>
#include <MyHeatTelebot.h>

#define XSTR(x) #x
#define STR(x) XSTR(x)

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

    MyHeatTelebot::begin(STR(TELEGRAM_BOT_TOKEN));
}

void loop()
{
    MyHeatTelebot::tick();
}