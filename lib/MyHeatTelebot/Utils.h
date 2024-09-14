#pragma once

#include <Arduino.h>
#include <StringUtils.h>
#include <FastBot2.h>
#include "MyHeatUtils.h"
#include "Menu.h"
#include "Message.h"
#include "User.h"
#include "CustomFunction.h"

namespace MyHeatTelebot
{
    String getConvertedFunctionToText(CustomFunction customFunction, byte functionIndex);
    Text getCallbackFromQuery(Text query);
    byte getValueFromQuery(String query);
    void setFunctionScreen(fb::TextEdit &msg, CustomFunction customFunction, byte functionIndex);
}