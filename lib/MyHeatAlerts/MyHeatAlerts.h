#ifndef MYHEATALERTS_H
#define MYHEATALERTS_H

#include <Arduino.h>
#include <vector>

typedef void (*AlertHandler)(const String& message);

namespace MyHeatAlerts
{
    void registerHandler(AlertHandler handler);
    void triggerAlert(const String& message);
}

#endif