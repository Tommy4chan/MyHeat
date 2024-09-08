#pragma once

#include <Arduino.h>
#include <StringUtils.h>
#include <FastBot2.h>
#include "MyHeatUtils.h"
#include "Menu.h"
#include "Message.h"
#include "User.h"

namespace MyHeatTelebot
{
    String getConvertedFunctionToText(byte functionIndex);
    Text getCallbackFromQuery(Text query);
    byte getValueFromQuery(String query);
    void setFunctionScreen(fb::TextEdit &msg, byte functionIndex);
}