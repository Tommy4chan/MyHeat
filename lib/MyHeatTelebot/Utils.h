#ifndef MYHEATTELEBOTUTILS_H
#define MYHEATTELEBOTUTILS_H

#include <Arduino.h>
#include <StringUtils.h>
#include <FastBot2.h>
#include "MyHeatUtils.h"
#include "Menu.h"
#include "Message.h"
#include "User.h"
#include "MyHeatCustomFunction.h"

namespace MyHeatTelebot
{
    String getConvertedFunctionToText(MyHeatCustomFunction customFunction, byte functionIndex);
    Text getCallbackFromQuery(Text query);
    byte getValueFromQuery(String query);
    void setFunctionScreen(fb::TextEdit &msg, MyHeatCustomFunction customFunction, byte functionIndex);
}

#endif
