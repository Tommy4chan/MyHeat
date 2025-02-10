#ifndef MYHEATWEBUTILS_H
#define MYHEATWEBUTILS_H

#include <ArduinoJson.h>

namespace MyHeatWeb
{
    void setErrorMessage(JsonObject status, String message);
    void setWarningMessage(JsonObject status, String message);
    void setSuccessMessage(JsonObject status, String message);
    void setInfoMessage(JsonObject status, String message);
}

#endif