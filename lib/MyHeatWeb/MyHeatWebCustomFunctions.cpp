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
            return;
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

        // payload is a JsonObject but we can create a JsonDocument to pass
        JsonDocument doc;
        doc.set(payload);
        
        myHeatDevice.customFunctions.manualDeserialize(doc);
        myHeatDevice.validateCustomFunctions();

        setSuccessMessage(status, "Функції збережено");
    }

    JsonDocument getFunctionsData()
    {
        JsonDocument functionsData;
        MyHeatDevice &myHeatDevice = MyHeatDevice::getInstance();

        functionsData[F("temperatureCount")] = myHeatDevice.temperatures.getTemperatureCount();
        functionsData[F("relayCount")] = myHeatDevice.relays.getRelayCount();
        functionsData[F("functionCount")] = myHeatDevice.customFunctions.getCustomFunctionCount();

        JsonArray functionsArray = functionsData[F("functions")].to<JsonArray>();
        
        for (int i = 0; i < myHeatDevice.customFunctions.getCustomFunctionCount(); i++)
        {
            MyHeatCustomFunction& function = myHeatDevice.customFunctions.getCustomFunction(i);
            JsonObject funcObj = functionsArray.add<JsonObject>();
            
            funcObj[F("name")] = function.getName();
            funcObj[F("isEnabled")] = function.getIsEnabled();
            funcObj[F("isActive")] = function.getIsActive();
            funcObj[F("priority")] = function.getPriority();

            JsonArray conditionsArr = funcObj[F("conditions")].to<JsonArray>();
            for (const auto& cond : function.getConditions()) {
                JsonObject condObj = conditionsArr.add<JsonObject>();
                condObj[F("t")] = cond.type;
                if (cond.type == COND_CMP) {
                    condObj[F("src")] = cond.source1;
                    condObj[F("si")] = cond.index1;
                    condObj[F("d1")] = cond.delta1;
                    condObj[F("op")] = cond.op;
                    condObj[F("ivs")] = cond.isValueSource;
                    if (cond.isValueSource) {
                        condObj[F("src2")] = cond.source2;
                        condObj[F("si2")] = cond.index2;
                        condObj[F("d2")] = cond.delta2_or_value;
                    } else {
                        condObj[F("v")] = cond.delta2_or_value;
                    }
                    condObj[F("hyst")] = cond.hysteresis;
                } else {
                    condObj[F("cs")] = cond.childStart;
                    condObj[F("cc")] = cond.childCount;
                }
            }

            JsonArray actionsArr = funcObj[F("actions")].to<JsonArray>();
            for (const auto& act : function.getActions()) {
                JsonObject actObj = actionsArr.add<JsonObject>();
                actObj[F("t")] = act.type;
                actObj[F("ti")] = act.targetIndex;
                actObj[F("v")] = act.value;
                if (act.type == ACT_SET_RELAY_TIMER) {
                    actObj[F("d")] = act.duration;
                    actObj[F("tt")] = act.timerType;
                }
            }
        }

        return functionsData;
    }
}
