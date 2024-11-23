#ifndef MYHEATTELEBOTMENU_H
#define MYHEATTELEBOTMENU_H

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
    fb::InlineMenu &getDiscoveredTemperatureSensorsInlineMenu(byte count, uint8_t addresses[][8]);
    fb::InlineMenu &getTemperatureIndexesInlineMenu();
    fb::InlineMenu &getFunctionListInlineMenu();
    fb::InlineMenu &getFunctionInlineMenu();
    fb::InlineMenu &getFunctionChangeSignInlineMenu();
    fb::InlineMenu &getFunctionChangeTemperatureInlineMenu();
    fb::InlineMenu &getFunctionChangeRelayInlineMenu();
    fb::InlineMenu &getCancelFunctionInlineMenu();
}
#endif
