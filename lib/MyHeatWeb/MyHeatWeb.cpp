#include "MyHeatWeb.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"

namespace MyHeatWeb
{
    void begin()
    {
        server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

        AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/api/wifi/set", [](AsyncWebServerRequest *request, JsonVariant &json) {
            const JsonObject& jsonObj = json.as<JsonObject>();
            myHeatWifi.setWifiCredentials(jsonObj["ssid"], jsonObj["password"]);
            request->send(200, "text/plain", "Wifi settings updated");
        });

        server.addHandler(handler);

        server.on("/api/wifi/get", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(200, "application/json", "{\"ssid\":\"" + myHeatWifi.getSSID() + "\",\"password\":\"" + myHeatWifi.getPassword() + "\"}"); 
        });

        server.begin();
    }
}