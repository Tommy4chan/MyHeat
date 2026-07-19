#ifndef MYHEATTELEBOTMENU_H
#define MYHEATTELEBOTMENU_H

#include <FastBot2.h>
#include <MyHeatUtils.h>
#include <vector>
#include <array>

using DeviceAddressArray = std::array<uint8_t, 8>;
#include "MyHeatRelay.h"
#include "MyHeatDevice.h"

namespace MyHeatTelebot
{
    inline fb::Menu replyMenu;
    inline fb::InlineMenu inlineMenu;

    fb::Menu &getMainReplyMenu();

    fb::InlineMenu &getRelayInlineMenu();
    fb::InlineMenu &getTemperatureInlineMenu();
    fb::InlineMenu &getDiscoveredTemperatureSensorsInlineMenu(byte count, std::vector<DeviceAddressArray>& addresses);
    fb::InlineMenu &getTemperatureIndexesInlineMenu();
    fb::InlineMenu &getFunctionListInlineMenu();
    fb::InlineMenu &getFunctionInlineMenu();
    fb::InlineMenu &getFunctionChangeSignInlineMenu();
    fb::InlineMenu &getFunctionChangeTemperatureInlineMenu();
    fb::InlineMenu &getFunctionChangeRelayInlineMenu();
    fb::InlineMenu &getCancelFunctionInlineMenu();
}

#endif
