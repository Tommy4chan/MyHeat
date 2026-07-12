#include "MyHeatAlerts.h"

namespace MyHeatAlerts
{
    static std::vector<AlertHandler> handlers;

    void registerHandler(AlertHandler handler)
    {
        handlers.push_back(handler);
    }

    void triggerAlert(const String& message)
    {
        for (auto handler : handlers)
        {
            handler(message);
        }
    }
}