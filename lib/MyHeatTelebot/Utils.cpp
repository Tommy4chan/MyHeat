#include "Utils.h"

namespace MyHeatTelebot
{
    String getConvertedFunctionToText(MyHeatCustomFunction customFunction, byte functionIndex)
    {
        String result[2] = {"", ""};

        for (int i = 0; i < 2; i++)
        {
            if (customFunction.getTemperatureIndex(i) == TN_INDEX)
            {
                result[i] += String(customFunction.getDeltaValue(i));
            }
            else
            {
                if (customFunction.getTemperatureIndex(i) == TEMP_UNKNOWN)
                {
                    result[i] += "Н/Д";
                }
                else
                {
                    result[i] += "T" + String(customFunction.getTemperatureIndex(i));
                }

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
        String isValidText = "";
        String relayText = "Реле " + String(customFunction.getRelayIndex());

        if (!customFunction.isValid())
        {
            isValidText = "Неіснуючий датчик температури або реле!!!\n";
        }

        if (customFunction.getRelayIndex() == RELAY_UNKNOWN)
        {
            relayText = "Н/Д";
        }

        return isValidText + "Функція " + String(functionIndex) + ": \n" +
               result[0] + " " + sign + " " + result[1] +
               "\nРеле: " + relayText +
               "\nСтан: " + MyHeatUtils::getConvertedStateToText(customFunction.getIsEnabled()) +
               "\nАктивна: " + MyHeatUtils::getConvertedActiveToText(customFunction.getIsActive()) + "\n\n";
    }

    Text getCallbackFromQuery(Text query)
    {
        return query.indexOf("_") == -1 ? query : query.substring(0, query.indexOf("_"));
    }

    byte getValueFromQuery(String query)
    {
        return query.substring(query.indexOf("_") + 1).toInt();
    }

    void setFunctionScreen(fb::TextEdit &msg, MyHeatCustomFunction customFunction, byte functionIndex)
    {
        setUserScreen(msg.chatID, ScreenType::FUNCTION_SCREEN);
        msg.text = getConvertedFunctionToText(customFunction, functionIndex);
        msg.setInlineMenu(getFunctionInlineMenu());
    }
}