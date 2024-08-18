#include "MyHeatUtils.h"

namespace MyHeatUtils
{
    String getConvertedStateToText(byte state)
    {
        String text = "";

        if (state == 0)
            text = "Вимкнено";
        else if (state == 1)
            text = "Увімкнено";
        else
            text = "Авто";

        return text;
    }

    String getConvertedActiveToText(bool active)
    {
        return active ? "Так" : "Ні";
    }

    String getCurrentDateAndTime()
    {
        time_t now = time(nullptr);
        char buffer[20];
        strftime(buffer, 20, "%d.%m.%Y %H:%M:%S", localtime(&now));
        return String(buffer) + "\n\n";
    }
}