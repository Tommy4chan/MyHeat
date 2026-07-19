#include "MyHeatWebCustomFunctions.h"

namespace MyHeatWeb
{
    void setFunctionIsEnabled(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        myHeatDevice.customFunctions.resetFunctionAlert(payload["functionIndex"]);

        if (payload["functionIndex"] >= myHeatDevice.customFunctions.getCustomFunctionCount())
        {
            setErrorMessage(status, F("Неіснуюча функція"));
        }
        if (myHeatDevice.customFunctions.setCustomFunctionIsEnabled(payload["functionIndex"], payload["isEnabled"])) {
            setSuccessMessage(status, "Стан функції " + payload["functionIndex"].as<String>() + " змінено");
            myHeatDevice.checkCustomFunctions();
        }
        else {
            setErrorMessage(status, "Не вдалося змінити стан функції " + payload["functionIndex"].as<String>() + ", перевірте її параметри");
        }
    }

    void setFunctionsSettings(JsonObject payload, JsonObject status)
    {
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        byte oldFunctionCount = myHeatDevice.customFunctions.getCustomFunctionCount();
        byte functionCount = payload["functionCount"];
        myHeatDevice.customFunctions.setCustomFunctionCount(functionCount);
        std::vector<MyHeatCustomFunction>& customFunctions = myHeatDevice.customFunctions.getCustomFunctions();

        byte temperatureCount = myHeatDevice.temperatures.getTemperatureCount();
        byte relayCount = myHeatDevice.relays.getRelayCount();

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

            customFunctions[i].setSign(static_cast<CustomFunctionSign>(payload["functions"][i]["sign"].as<byte>()));
            customFunctions[i].setTemperatureIndex(0, payload["functions"][i]["temperatureIndex"][0]);
            customFunctions[i].setTemperatureIndex(1, payload["functions"][i]["temperatureIndex"][1]);
            customFunctions[i].setDeltaValue(0, payload["functions"][i]["deltaValue"][0]);
            customFunctions[i].setDeltaValue(1, payload["functions"][i]["deltaValue"][1]);
            customFunctions[i].setRelayIndex(payload["functions"][i]["relayIndex"]);
        }

        myHeatDevice.customFunctions.save();
        myHeatDevice.validateCustomFunctions();

        setSuccessMessage(status, "Функції збережено");
    }

    JsonDocument getFunctionsData()
    {
        JsonDocument functionsData;
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        functionsData[F("temperatureCount")] = myHeatDevice.temperatures.getTemperatureCount();
        functionsData[F("relayCount")] = myHeatDevice.relays.getRelayCount();

        for (int i = 0; i < myHeatDevice.customFunctions.getCustomFunctionCount(); i++)
        {
            MyHeatCustomFunction function = myHeatDevice.customFunctions.getCustomFunction(i);

            functionsData[F("functions")][i][F("sign")] = static_cast<byte>(function.getSign());
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
