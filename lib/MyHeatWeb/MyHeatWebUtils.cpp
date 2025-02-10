#include "MyHeatWebUtils.h"

namespace MyHeatWeb
{
    void setErrorMessage(JsonObject status, String message)
    {
        status["message"] = message;
        status["type"] = "error";
    }

    void setWarningMessage(JsonObject status, String message)
    {
        status["message"] = message;
        status["type"] = "warning";
    }

    void setSuccessMessage(JsonObject status, String message)
    {
        status["message"] = message;
        status["type"] = "success";
    }

    void setInfoMessage(JsonObject status, String message)
    {
        status["message"] = message;
        status["type"] = "info";
    }
}