#ifndef MYHEATTELEBOTMESSAGE_H
#define MYHEATTELEBOTMESSAGE_H

#include <StringUtils.h>
#include "MyHeatUtils.h"
#include "Utils.h"
#include "MyHeatCustomFunction.h"
#include "MyHeatRelay.h"

namespace MyHeatTelebot
{
    String getRelayScreenText(MyHeatRelay *relaysPtr);
    String getTemperatureScreenText(float temperatures[]);
    String getFunctionsListScreenText();
    String getFunctionScreenText(MyHeatCustomFunction customFunction, byte functionIndex);
}

#endif
