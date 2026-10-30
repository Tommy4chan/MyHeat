#include "Utils.h"

namespace MyHeatTelebot
{
    String getConvertedFunctionToText(const MyHeatCustomFunction& customFunction, byte functionIndex)
    {
        String finalResult = "";
        finalResult.reserve(200);
        finalResult += "Функція ";
        finalResult += String(functionIndex);
        if (strlen(customFunction.getName()) > 0) {
            finalResult += " (";
            finalResult += customFunction.getName();
            finalResult += ")";
        }
        finalResult += ": \n";
        
        finalResult += "Стан: ";
        finalResult += MyHeatUtils::getConvertedStateToText(customFunction.getIsEnabled());
        finalResult += "\nАктивна: ";
        finalResult += MyHeatUtils::getConvertedActiveToText(customFunction.getIsActive());
        finalResult += "\nПріоритет: ";
        finalResult += String(customFunction.getPriority());
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
        msg.text = getConvertedFunctionToText(customFunction, functionIndex);
        msg.setInlineMenu(getFunctionInlineMenu(functionIndex));
    }

    // Telegram HTML mode only requires these three characters to be escaped
    String escapeHtml(const String& text)
    {
        String escaped;
        escaped.reserve(text.length());
        for (char c : text)
        {
            switch (c)
            {
            case '&': escaped += F("&amp;"); break;
            case '<': escaped += F("&lt;"); break;
            case '>': escaped += F("&gt;"); break;
            default: escaped += c;
            }
        }
        return escaped;
    }
}