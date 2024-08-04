#pragma once
#include <FastBot2.h>
#include "Menu.h"
#include "Message.h"
#include "User.h"

namespace MyHeatTelebot
{
    inline FastBot2 bot;

    void begin(String token);
    void tick();

    void handleUpdate(fb::Update& u);
    void handleMessage(fb::Update& u);
    void handleCommand(fb::Update& u);
    void handleQuery(fb::Update& u);
    void handleUserRegistration(fb::Update &u);
}