#include "MyHeatWebCustomFunctions.h"

namespace MyHeatWeb
{
    void setFunctionIsEnabled(JsonObject payload)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        myHeatDevice.setCustomFunctionIsEnabled(payload["functionIndex"], payload["isEnabled"]);
        myHeatDevice.checkCustomFunctions();
    }

    void setFunctionsSettings(JsonObject payload)
    {
        MyHeatDevice::getInstance().updateFunctionsSettings(payload);
    }

    JsonDocument getFunctionsData()
    {
        JsonDocument functionsData;
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        functionsData[F("temperatureCount")] = myHeatDevice.getTemperatureCount();
        functionsData[F("relayCount")] = myHeatDevice.getRelayCount();

        for (int i = 0; i < myHeatDevice.getCustomFunctionCount(); i++)
        {
            MyHeatCustomFunction function = myHeatDevice.getCustomFunction(i);

            functionsData[F("functions")][i][F("sign")] = function.getSign();
            functionsData[F("functions")][i][F("temperatureIndex")][0] = function.getTemperatureIndex(0);
            functionsData[F("functions")][i][F("temperatureIndex")][1] = function.getTemperatureIndex(1);
            functionsData[F("functions")][i][F("deltaValue")][0] = function.getDeltaValue(0);
            functionsData[F("functions")][i][F("deltaValue")][1] = function.getDeltaValue(1);
            functionsData[F("functions")][i][F("relayIndex")] = function.getRelayIndex();
            functionsData[F("functions")][i][F("isEnabled")] = function.getIsEnabled();
            functionsData[F("functions")][i][F("isActive")] = function.getIsActive();
        }

        return functionsData;
    }
}
