#ifndef MYHEATALERTS_H
#define MYHEATALERTS_H

#include <Arduino.h>
#include <vector>

enum class AlertType : byte
{
    TEMPERATURE_LOW,
    TEMPERATURE_HIGH,
    TEMPERATURE_DISCONNECTED,
    FUNCTION_BAD_TEMPERATURE,
    FUNCTION_MISSING_TEMPERATURE,
    FUNCTION_MISSING_RELAY,
    SMOKE_DETECTED,
    SMOKE_SENSOR_DISCONNECTED,
    COUNT
};

struct AlertEvent
{
    AlertType type;
    int index; // Sensor or function index, -1 when the alert is not tied to one
    bool isResolved;
    const String &message;
};

typedef void (*AlertHandler)(const String& message);
typedef void (*AlertEventHandler)(const AlertEvent& event);

namespace MyHeatAlerts
{
    // Text handlers receive raised alerts only (Telegram, web UI)
    void registerHandler(AlertHandler handler);
    // Event handlers receive raised and resolved alerts with their type (Home Assistant)
    void registerEventHandler(AlertEventHandler handler);

    void triggerAlert(AlertType type, int index, const String& message);
    void resolveAlert(AlertType type, int index, const String& message);

    // Stable machine-readable name, used as the HA event type
    const char* getAlertTypeName(AlertType type);
}

#endif
