#ifndef MYHEATTELEBOT_H
#define MYHEATTELEBOT_H

#include <FastBot2.h>

namespace MyHeatTelebot
{
    extern FastBot2 bot;

    void begin(String token);
    void tick();

    void handleUpdate(fb::Update& u);
    void handleMessage(fb::Update& u);
    void handleCommand(fb::Update& u);
    void handleQuery(fb::Update& u);
}

#endif