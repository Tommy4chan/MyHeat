#include "Utils.h"

namespace MyHeatTelebot
{
    String getConvertedFunctionToText(MyHeatCustomFunction customFunction, byte functionIndex)
    {
        String result[2] = {"", ""};

        for (int i = 0; i < 2; i++)
        {
            if (customFunction.getTemperatureIndex(i) == TEMPERATURE_COUNT)
            {
                result[i] += String(customFunction.getDeltaValue(i));
            }
            else
            {
                result[i] += "T" + String(customFunction.getTemperatureIndex(i));
                if (customFunction.getDeltaValue(i) != 0)
                {
                    if (customFunction.getDeltaValue(i) > 0)
                        result[i] += " + ";
                    else
                        result[i] += " - ";
                    result[i] += String(abs(customFunction.getDeltaValue(i)));
                }
            }
        }

        String sign(char(60 + customFunction.getSign()));

        return "Функція " + String(functionIndex + 1) + ": \n" +
               result[0] + " " + sign + " " + result[1] +
               "\nРеле: Реле " + String(customFunction.getRelayIndex()) + 
               "\nСтан: " + MyHeatUtils::getConvertedStateToText(customFunction.getIsEnabled()) +
               "\nАктивна: " + MyHeatUtils::getConvertedActiveToText(customFunction.getIsActive()) + "\n\n";
    }

    Text getCallbackFromQuery(Text query) {
        return query.indexOf("_") == -1 ? query : query.substring(0, query.indexOf("_"));
    }

    byte getValueFromQuery(String query) {
        return query.substring(query.indexOf("_") + 1).toInt();
    }

    void setFunctionScreen(fb::TextEdit &msg, MyHeatCustomFunction customFunction, byte functionIndex) {
        setUserScreen(msg.chatID, ScreenType::FUNCTION_SCREEN);
        msg.text = getFunctionScreenText(customFunction, functionIndex);
        msg.setInlineMenu(getFunctionInlineMenu());
    }
}