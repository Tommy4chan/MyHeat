#include "MyHeatWebRelays.h"

namespace MyHeatWeb
{
    void getRelaysSettings(JsonDocument &response)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (int i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            response["payload"]["relays"][i]["pin"] = myHeatDevice.getRelay(i).getPin();
            response["payload"]["relays"][i]["isActiveOnHigh"] = myHeatDevice.getRelay(i).getIsActiveOnHigh();
        }
    }

    void setRelayMode(JsonObject payload)
    {
        MyHeatDevice::getInstance().setRelayMode(payload["relayIndex"], payload["relayMode"]);
    }

    void setRelaysSettings(JsonObject payload)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        for (int i = 0; i < myHeatDevice.getRelayCount(); i++)
        {
            myHeatDevice.setRelaySettings(i, payload["relays"][i]["pin"], payload["relays"][i]["isActiveOnHigh"], false);
        }

        myHeatDevice.MyHeatRelays::save();
    }

    void setRelayCount(JsonObject payload)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        myHeatDevice.setRelayCount(payload["relayCount"]);

        myHeatDevice.initIsSetRelayActive();
        MyHeatHardwareIO::getInstance().reevaluateScreensCount();
        myHeatDevice.validateCustomFunctions();
    }

    void getRelayCount(JsonDocument &response)
    {
        response["payload"]["relayCount"] = MyHeatDevice::getInstance().getRelayCount();
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
