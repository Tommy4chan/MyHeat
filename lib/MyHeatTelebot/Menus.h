#pragma once
#include <FastBot2.h>

namespace MyHeatTelebot
{
    inline fb::Menu replyMenu;
    inline fb::InlineMenu inlineMenu;

    fb::Menu &getMainReplyMenu();

    fb::InlineMenu &getRelayInlineMenu();
}