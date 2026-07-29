#ifndef MYHEATTELEBOTMENU_H
#define MYHEATTELEBOTMENU_H

#include <FastBot2.h>
#include <MyHeatUtils.h>
#include "MyHeatRelay.h"
#include "MyHeatDevice.h"

namespace MyHeatTelebot
{
    inline fb::Menu replyMenu;
    inline fb::InlineMenu inlineMenu;

    fb::Menu &getMainReplyMenu();

    fb::InlineMenu &getRelayInlineMenu();
    fb::InlineMenu &getTemperatureInlineMenu();
    fb::InlineMenu &getFunctionListInlineMenu();
    fb::InlineMenu &getFunctionInlineMenu(byte functionIndex);
}

#endif
