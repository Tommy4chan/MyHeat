#include "MyHeatWebCustomFunctions.h"

namespace MyHeatWeb
{
    void setFunctionIsEnabled(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();
        if (payload["functionIndex"] >= myHeatDevice.getCustomFunctionCount())
        {
            setErrorMessage(status, F("Неіснуюча функція"));
        }
        myHeatDevice.setCustomFunctionIsEnabled(payload["functionIndex"], payload["isEnabled"]);
        myHeatDevice.checkCustomFunctions();
    }

    void setFunctionsSettings(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        byte oldFunctionCount = myHeatDevice.getCustomFunctionCount();
        byte functionCount = payload["functionCount"];
        myHeatDevice.setCustomFunctionCount(functionCount);
        MyHeatCustomFunction *customFunctions = myHeatDevice.getCustomFunctions();

        byte temperatureCount = myHeatDevice.getTemperatureCount();
        byte relayCount = myHeatDevice.getRelayCount();

        for (int i = 0; i < functionCount; i++)
        {
            if (payload["functions"][i]["temperatureIndex"][0] >= temperatureCount && payload["functions"][i]["temperatureIndex"][0] < TEMP_UNKNOWN) 
            {
                setErrorMessage(status, "Неправильний індекс температурного датчика в Параметрі 1 в функції " + String(i));
                continue;
            }
            else if (payload["functions"][i]["temperatureIndex"][1] >= temperatureCount && payload["functions"][i]["temperatureIndex"][1] < TEMP_UNKNOWN)
            {
                setErrorMessage(status, "Неправильний індекс температурного датчика в Параметрі 2 в функції " + String(i));
                continue;
            }
            else if (payload["functions"][i]["relayIndex"] >= relayCount && payload["functions"][i]["relayIndex"] != RELAY_UNKNOWN)
            {
                setErrorMessage(status, "Неправильний індекс реле в функції " + String(i));
                continue;
            }

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
