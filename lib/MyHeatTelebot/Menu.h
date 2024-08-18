#pragma once

#include <FastBot2.h>
#include "MyHeatUtils.h"

namespace MyHeatTelebot
{
    inline fb::Menu replyMenu;
    inline fb::InlineMenu inlineMenu;

    fb::Menu &getMainReplyMenu();

    fb::InlineMenu &getRelayInlineMenu();
    fb::InlineMenu &getTemperatureInlineMenu();
    fb::InlineMenu &getFunctionInlineMenu();
}