#ifndef MYHEATENUMS_H
#define MYHEATENUMS_H

#include <Arduino.h>

enum class RelayMode : byte {
    OFF = 0,
    ON = 1,
    AUTO = 2
};

enum class CustomFunctionSign : byte {
    LESS = 0,
    EQUAL = 1,
    GREATER = 2
};

#endif
