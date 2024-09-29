#pragma once
#include <FastBot2.h>
#include "Menu.h"
#include "Message.h"
#include "User.h"
#include "MyHeatDevice.h"
#include "MyHeatUtils.h"

namespace MyHeatTelebot
{
    inline FastBot2 bot;
    inline MyHeatDevice *myHeatDevicePtr = nullptr;

    void begin(String token, MyHeatDevice *myHeatDevicePtr);
    void tick();

    void handleUpdate(fb::Update &u);
    void handleMessage(fb::Update &u);
    void handleCommand(fb::Update &u);
    void handleQuery(fb::Update &u);
    void handleUserRegistration(fb::Update &u);
    void handleUserInput(fb::Update &u);
}