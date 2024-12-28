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
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        byte oldFunctionCount = myHeatDevice.getCustomFunctionCount();
        byte functionCount = payload["functionCount"];
        myHeatDevice.setCustomFunctionCount(functionCount);
        MyHeatCustomFunction *customFunctions = myHeatDevice.getCustomFunctions();

        for (int i = 0; i < functionCount; i++)
        {
            customFunctions[i].setSign(payload["functions"][i]["sign"]);
            customFunctions[i].setTemperatureIndex(0, payload["functions"][i]["temperatureIndex"][0]);
            customFunctions[i].setTemperatureIndex(1, payload["functions"][i]["temperatureIndex"][1]);
            customFunctions[i].setDeltaValue(0, payload["functions"][i]["deltaValue"][0]);
            customFunctions[i].setDeltaValue(1, payload["functions"][i]["deltaValue"][1]);
            customFunctions[i].setRelayIndex(payload["functions"][i]["relayIndex"]);
        }

        myHeatDevice.MyHeatCustomFunctions::save();
        myHeatDevice.validateCustomFunctions();
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