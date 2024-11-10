#include "MyHeatWeb.h"
#include "ArduinoJson.h"
#include "AsyncJson.h"

namespace MyHeatWeb
{
    void begin(MyHeatDevice *myHeatDevicePtr, MyHeatWifi *myHeatWifiPtr)
    {
        MyHeatWeb::myHeatDevicePtr = myHeatDevicePtr;
        MyHeatWeb::myHeatWifiPtr = myHeatWifiPtr;

        server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

        AsyncCallbackJsonWebHandler *handler = new AsyncCallbackJsonWebHandler("/api/wifi/set", [myHeatWifiPtr](AsyncWebServerRequest *request, JsonVariant &json) {
            const JsonObject& jsonObj = json.as<JsonObject>();
            myHeatWifiPtr->setWifiCredentials(jsonObj["ssid"], jsonObj["password"]);
            request->send(200, "text/plain", "Wifi settings updated");
        });

        server.addHandler(handler);

        server.on("/api/wifi/get", HTTP_GET, [myHeatWifiPtr](AsyncWebServerRequest *request) {
            request->send(200, "application/json", "{\"ssid\":\"" + myHeatWifiPtr->getSSID() + "\",\"password\":\"" + myHeatWifiPtr->getPassword() + "\"}"); 
        });

        server.begin();
    }
}