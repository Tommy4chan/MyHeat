#include "MyHeatWebRelays.h"

namespace MyHeatWeb
{
    void getRelaysSettings(JsonObject response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (int i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            response["relays"][i]["pin"] = myHeatDevice.getRelay(i).getPin();
            response["relays"][i]["isActiveOnHigh"] = myHeatDevice.getRelay(i).getIsActiveOnHigh();
        }
    }

    void setRelayMode(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        if (payload["relayIndex"] >= myHeatDevice.getRelayCount())
        {
            setErrorMessage(status, F("Неіснуюче реле"));
            return;
        }

        setSuccessMessage(status, "Змінено статус Реле " + String(payload["relayIndex"]));

        myHeatDevice.setRelayMode(payload["relayIndex"], payload["relayMode"]);
    }

    void setRelaysSettings(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (int i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            myHeatDevice.setRelaySettings(i, payload["relays"][i]["pin"], payload["relays"][i]["isActiveOnHigh"], false);
        }

        setSuccessMessage(status, "Налаштування реле збережено");

        myHeatDevice.MyHeatRelays::save();
    }

    void setRelayCount(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        myHeatDevice.setRelayCount(payload["relayCount"]);

        myHeatDevice.initIsSetRelayActive();
        MyHeatHardwareIO::getInstance().reevaluateScreensCount();
        myHeatDevice.validateCustomFunctions();

        setSuccessMessage(status, "Кількість реле змінено");
    }

    void getRelayCount(JsonObject response)
    {
        response["relayCount"] = MyHeatDevice::getInstance().getRelayCount();
    }

    JsonDocument getRelaysData()
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        JsonDocument relaysData;

        for (int i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            MyHeatRelay relay = myHeatDevice.getRelay(i);
            relaysData[F("relays")][i][F("mode")] = relay.getMode();
            relaysData[F("relays")][i][F("isActive")] = relay.getIsActive();
        }

        return relaysData;
    }
}
