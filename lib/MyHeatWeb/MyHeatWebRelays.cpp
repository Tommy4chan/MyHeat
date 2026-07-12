#include "MyHeatWebRelays.h"

namespace MyHeatWeb
{
    void getRelaysSettings(JsonObject response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (int i = 0; i < myHeatDevice.relays.getRelayCount(); i++)
        {
            response["relays"][i]["pin"] = myHeatDevice.relays.getRelay(i).getPin();
            response["relays"][i]["isActiveOnHigh"] = myHeatDevice.relays.getRelay(i).getIsActiveOnHigh();
        }
    }

    void setRelayMode(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        if (payload["relayIndex"] >= myHeatDevice.relays.getRelayCount())
        {
            setErrorMessage(status, F("Неіснуюче реле"));
            return;
        }

        setSuccessMessage(status, "Змінено статус Реле " + String(payload["relayIndex"]));

        myHeatDevice.relays.setRelayMode(payload["relayIndex"], static_cast<RelayMode>(payload["relayMode"].as<byte>()));
    }

    void setRelaysSettings(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (int i = 0; i < myHeatDevice.relays.getRelayCount(); i++)
        {
            myHeatDevice.relays.setRelaySettings(i, payload["relays"][i]["pin"], payload["relays"][i]["isActiveOnHigh"], false);
        }

        setSuccessMessage(status, "Налаштування реле збережено");

        myHeatDevice.relays.save();
    }

    void setRelayCount(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        myHeatDevice.relays.setRelayCount(payload["relayCount"]);

        myHeatDevice.initIsSetRelayActive();
        MyHeatHardwareIO::getInstance().reevaluateScreensCount();
        myHeatDevice.validateCustomFunctions();

        setSuccessMessage(status, "Кількість реле змінено");
    }

    void getRelayCount(JsonObject response)
    {
        response["relayCount"] = MyHeatDevice::getInstance().relays.getRelayCount();
    }

    JsonDocument getRelaysData()
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        JsonDocument relaysData;

        for (int i = 0; i < myHeatDevice.relays.getRelayCount(); i++)
        {
            MyHeatRelay relay = myHeatDevice.relays.getRelay(i);
            relaysData[F("relays")][i][F("mode")] = static_cast<byte>(relay.getMode());
            relaysData[F("relays")][i][F("isActive")] = relay.getIsActive();
        }

        return relaysData;
    }
}
