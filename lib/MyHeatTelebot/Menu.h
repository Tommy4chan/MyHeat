#pragma once

#include <FastBot2.h>
#include "MyHeatUtils.h"
#include "MyHeatRelay.h"

namespace MyHeatTelebot
{
    inline fb::Menu replyMenu;
    inline fb::InlineMenu inlineMenu;

    fb::Menu &getMainReplyMenu();

    fb::InlineMenu &getRelayInlineMenu(MyHeatRelay *relaysPtr);
    fb::InlineMenu &getTemperatureInlineMenu();
    fb::InlineMenu &getFunctionListInlineMenu();
    fb::InlineMenu &getFunctionInlineMenu();
    fb::InlineMenu &getFunctionChangeSignInlineMenu();
    fb::InlineMenu &getFunctionChangeTemperatureInlineMenu();
    fb::InlineMenu &getFunctionChangeRelayInlineMenu();
    fb::InlineMenu &getCancelFunctionInlineMenu();
}