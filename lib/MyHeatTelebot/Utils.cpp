#include "Utils.h"

namespace MyHeatTelebot
{
    String getConvertedFunctionToText(const MyHeatCustomFunction& customFunction, byte functionIndex)
    {
        String result[2] = {"", ""};
        result[0].reserve(30);
        result[1].reserve(30);

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

        String sign(char(60 + static_cast<byte>(customFunction.getSign())));
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

        String finalResult = "";
        finalResult.reserve(200);
        finalResult += isValidText;
        finalResult += "Функція ";
        finalResult += String(functionIndex);
        finalResult += ": \n";
        finalResult += result[0];
        finalResult += " ";
        finalResult += sign;
        finalResult += " ";
        finalResult += result[1];
        finalResult += "\nРеле: ";
        finalResult += relayText;
        finalResult += "\nСтан: ";
        finalResult += MyHeatUtils::getConvertedStateToText(customFunction.getIsEnabled());
        finalResult += "\nАктивна: ";
        finalResult += MyHeatUtils::getConvertedActiveToText(customFunction.getIsActive());
        finalResult += "\n\n";
        
        return finalResult;
    }

    Text getCallbackFromQuery(Text query)
    {
        return query.indexOf("_") == -1 ? query : query.substring(0, query.indexOf("_"));
    }

    byte getValueFromQuery(const String& query)
    {
        return query.substring(query.indexOf("_") + 1).toInt();
    }

    void setFunctionScreen(fb::TextEdit &msg, const MyHeatCustomFunction& customFunction, byte functionIndex)
    {
        setUserScreen((Text)msg.chatID, ScreenType::FUNCTION_SCREEN);
        msg.text = getConvertedFunctionToText(customFunction, functionIndex);
        msg.setInlineMenu(getFunctionInlineMenu());
    }
}