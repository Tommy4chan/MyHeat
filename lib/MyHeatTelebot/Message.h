#ifndef MYHEATTELEBOTMESSAGE_H
#define MYHEATTELEBOTMESSAGE_H

#include <StringUtils.h>
#include "MyHeatUtils.h"
#include "Utils.h"
#include "MyHeatCustomFunction.h"
#include "MyHeatRelay.h"

namespace MyHeatTelebot
{
    String getRelayScreenText();
    String getTemperatureScreenText();
    String getFunctionsListScreenText();

}

#endif
