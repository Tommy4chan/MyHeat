#include "MyHeatAlerts.h"

namespace MyHeatAlerts
{
    static std::vector<AlertHandler> handlers;
    static std::vector<AlertEventHandler> eventHandlers;

    static const char *ALERT_TYPE_NAMES[] = {
        "temperature_low",
        "temperature_high",
        "temperature_disconnected",
        "function_bad_temperature",
        "function_missing_temperature",
        "function_missing_relay",
        "smoke_detected",
        "smoke_sensor_disconnected",
    };
    static_assert(sizeof(ALERT_TYPE_NAMES) / sizeof(ALERT_TYPE_NAMES[0]) == static_cast<size_t>(AlertType::COUNT),
                  "Every AlertType needs a name");

    void registerHandler(AlertHandler handler)
    {
        handlers.push_back(handler);
    }

    void registerEventHandler(AlertEventHandler handler)
    {
        eventHandlers.push_back(handler);
    }

    static void dispatchEvent(AlertType type, int index, bool isResolved, const String& message)
    {
        AlertEvent event{type, index, isResolved, message};
        for (auto handler : eventHandlers)
        {
            handler(event);
        }
    }

    void triggerAlert(AlertType type, int index, const String& message)
    {
        for (auto handler : handlers)
        {
            handler(message);
        }

        dispatchEvent(type, index, false, message);
    }

    void resolveAlert(AlertType type, int index, const String& message)
    {
        dispatchEvent(type, index, true, message);
    }

    const char* getAlertTypeName(AlertType type)
    {
        size_t typeIndex = static_cast<size_t>(type);
        return typeIndex < static_cast<size_t>(AlertType::COUNT) ? ALERT_TYPE_NAMES[typeIndex] : "unknown";
    }
}
