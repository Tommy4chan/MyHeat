#include "Utils.h"

namespace MyHeatTelebot
{
    String getConvertedFunctionToText(byte functionIndex)
    {
        //temp data for testing
        struct CustomFunction
        {
            byte sign;
            byte temp[2];
            float deltaValue[2];
            byte relayNumber;
            bool isEnabled;
            bool isActive;
        };

        //temp data for testing
        CustomFunction customFunctions[FUNCTION_COUNT] = {
            {0, {0, 1}, {23, 0}, 0, false, false},
            {1, {2, 0}, {0, -12}, 1, true, false},
            {2, {3, 4}, {0, 0}, 1, true, true},
            {0, {6, 0}, {0, 0}, 0, false, false}};

        String result[2] = {"", ""};

        for (int i = 0; i < 2; i++)
        {
            if (customFunctions[functionIndex].temp[i] == TEMPERATURE_COUNT)
            {
                result[i] += String(customFunctions[functionIndex].deltaValue[i]);
            }
            else
            {
                result[i] += "T" + String(customFunctions[functionIndex].temp[i]);
                if (customFunctions[functionIndex].deltaValue[i] != 0)
                {
                    if (customFunctions[functionIndex].deltaValue[i] > 0)
                        result[i] += " + ";
                    else
                        result[i] += " - ";
                    result[i] += String(abs(customFunctions[functionIndex].deltaValue[i]));
                }
            }
        }

        String sign(char(60 + customFunctions[functionIndex].sign));

        return "Функція " + String(functionIndex) + ": \n" +
               result[0] + " " + sign + " " + result[1] +
               "\nРеле: Реле " + String(customFunctions[functionIndex].relayNumber) + 
               "\nСтан: " + MyHeatUtils::getConvertedStateToText(customFunctions[functionIndex].isEnabled) +
               "\nАктивна: " + MyHeatUtils::getConvertedActiveToText(customFunctions[functionIndex].isActive) + "\n\n";
    }
}