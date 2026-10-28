#include "MyHeatCustomFunctions.h"
#include <MyHeatAlerts.h>
#include "MyHeatRelay.h"
#include "MyHeatSmokeSensor.h"
#include <cmath>

void MyHeatCustomFunctions::serialize(JsonDocument &doc)
{
    doc[F("functionCount")] = customFunctions.size();
    
    JsonArray functionsArray = doc[F("functions")].to<JsonArray>();

    for (size_t i = 0; i < customFunctions.size(); i++)
    {
        JsonObject funcObj = functionsArray.add<JsonObject>();
        funcObj[F("name")] = customFunctions[i].getName();
        funcObj[F("isEnabled")] = customFunctions[i].getIsEnabled();
        funcObj[F("priority")] = customFunctions[i].getPriority();

        JsonArray conditionsArr = funcObj[F("conditions")].to<JsonArray>();
        const auto& conditions = customFunctions[i].getConditions();
        for (const auto& cond : conditions) {
            JsonObject condObj = conditionsArr.add<JsonObject>();
            condObj[F("t")] = cond.type;
            if (cond.type == COND_CMP) {
                condObj[F("src")] = cond.source1;
                condObj[F("si")] = cond.index1;
                condObj[F("d1")] = cond.delta1;
                condObj[F("op")] = cond.op;
                condObj[F("ivs")] = cond.isValueSource;
                if (cond.isValueSource) {
                    condObj[F("src2")] = cond.source2;
                    condObj[F("si2")] = cond.index2;
                    condObj[F("d2")] = cond.delta2_or_value;
                } else {
                    condObj[F("v")] = cond.delta2_or_value;
                }
                condObj[F("hyst")] = cond.hysteresis;
            } else {
                condObj[F("cs")] = cond.childStart;
                condObj[F("cc")] = cond.childCount;
            }
        }

        JsonArray actionsArr = funcObj[F("actions")].to<JsonArray>();
        const auto& actions = customFunctions[i].getActions();
        for (const auto& act : actions) {
            JsonObject actObj = actionsArr.add<JsonObject>();
            actObj[F("t")] = act.type;
            actObj[F("ti")] = act.targetIndex;
            actObj[F("v")] = act.value;
            if (act.type == ACT_SET_RELAY_TIMER) {
                actObj[F("d")] = act.duration;
                actObj[F("tt")] = act.timerType;
            }
        }
    }
}

void MyHeatCustomFunctions::deserialize(const JsonDocument &doc)
{
    realocateMemory(doc[F("functionCount")] | FUNCTION_COUNT);

    JsonArrayConst functionsArray = doc[F("functions")];
    size_t count = min((size_t)customFunctions.size(), functionsArray.size());

    for (size_t i = 0; i < count; i++)
    {
        JsonObjectConst funcObj = functionsArray[i];
        
        if (!funcObj[F("name")].isNull()) {
            customFunctions[i].setName(funcObj[F("name")].as<const char*>());
        }
        customFunctions[i].setIsEnabled(funcObj[F("isEnabled")] | false);
        customFunctions[i].setPriority(funcObj[F("priority")] | 0);

        auto& conditions = customFunctions[i].getConditions();
        conditions.clear();
        JsonArrayConst conditionsArr = funcObj[F("conditions")];
        for (JsonObjectConst condObj : conditionsArr) {
            ConditionNode cond;
            memset(&cond, 0, sizeof(ConditionNode));
            cond.type = static_cast<ConditionType>(condObj[F("t")].as<uint8_t>());
            
            if (cond.type == COND_CMP) {
                cond.source1 = static_cast<SourceType>(condObj[F("src")].as<uint8_t>());
                cond.index1 = condObj[F("si")] | 0;
                cond.delta1 = condObj[F("d1")] | 0.0f;
                cond.op = static_cast<CompareOp>(condObj[F("op")].as<uint8_t>());
                cond.isValueSource = condObj[F("ivs")] | false;
                
                if (cond.isValueSource) {
                    cond.source2 = static_cast<SourceType>(condObj[F("src2")].as<uint8_t>());
                    cond.index2 = condObj[F("si2")] | 0;
                    cond.delta2_or_value = condObj[F("d2")] | 0.0f;
                } else {
                    cond.delta2_or_value = condObj[F("v")] | 0.0f;
                }
                cond.hysteresis = condObj[F("hyst")] | 0.0f;
            } else {
                cond.childStart = condObj[F("cs")] | 0;
                cond.childCount = condObj[F("cc")] | 0;
            }
            cond.lastResult = -1;
            conditions.push_back(cond);
        }

        auto& actions = customFunctions[i].getActions();
        actions.clear();
        JsonArrayConst actionsArr = funcObj[F("actions")];
        for (JsonObjectConst actObj : actionsArr) {
            ActionNode act;
            memset(&act, 0, sizeof(ActionNode));
            act.type = static_cast<ActionType>(actObj[F("t")].as<uint8_t>());
            act.targetIndex = actObj[F("ti")] | 0;
            act.value = actObj[F("v")] | 0;
            if (act.type == ACT_SET_RELAY_TIMER) {
                act.duration = actObj[F("d")] | 0;
                act.timerType = actObj[F("tt")] | 0;
            }
            actions.push_back(act);
        }
    }
}

void MyHeatCustomFunctions::realocateMemory(byte newFunctionCount)
{
    customFunctions.resize(newFunctionCount);
    functionAlerts.resize(newFunctionCount, FunctionAlert::FA_NONE);
}

MyHeatCustomFunctions::MyHeatCustomFunctions()
{
    realocateMemory(FUNCTION_COUNT);
}

void MyHeatCustomFunctions::begin()
{
    customFunctionsData = new MyHeatSave("/customFunctions.json", this);
    customFunctionsData->read();
}

MyHeatCustomFunction& MyHeatCustomFunctions::getCustomFunction(byte index)
{
    return customFunctions[index];
}

std::vector<MyHeatCustomFunction>& MyHeatCustomFunctions::getCustomFunctions()
{
    return customFunctions;
}

void MyHeatCustomFunctions::toggleCustomFunctionIsEnabled(byte functionIndex)
{
    customFunctions[functionIndex].toggleIsEnabled();
    save();
}

bool MyHeatCustomFunctions::setCustomFunctionIsEnabled(byte functionIndex, bool isEnabled)
{
    customFunctions[functionIndex].setIsEnabled(isEnabled);
    save();
    return true;
}

void MyHeatCustomFunctions::setCustomFunctionCount(byte newCustomFunctionCount)
{
    realocateMemory(newCustomFunctionCount);
    save();
}

byte MyHeatCustomFunctions::getCustomFunctionCount()
{
    return customFunctions.size();
}

void MyHeatCustomFunctions::manualDeserialize(const JsonDocument& payload)
{
    deserialize(payload);
    save();
}

static AlertType toAlertType(FunctionAlert alert)
{
    switch (alert)
    {
    case FunctionAlert::FA_BAD_TEMPERATURE_NUMBER:
        return AlertType::FUNCTION_MISSING_TEMPERATURE;
    case FunctionAlert::FA_BAD_RELAY_NUMBER:
        return AlertType::FUNCTION_MISSING_RELAY;
    default:
        return AlertType::FUNCTION_BAD_TEMPERATURE;
    }
}

static String getFunctionAlertText(FunctionAlert alert, byte functionIndex, bool isResolved)
{
    String function = "Функція " + String(functionIndex);
    switch (alert)
    {
    case FunctionAlert::FA_BAD_TEMPERATURE_NUMBER:
        return function + (isResolved ? ": неіснуючий датчик температури більше не використовується" : " не може бути активною через використання неіснуючого датчика температури");
    case FunctionAlert::FA_BAD_RELAY_NUMBER:
        return function + (isResolved ? ": неіснуюче реле більше не використовується" : " не може бути активною через використання неіснуючого реле");
    default:
        return function + (isResolved ? ": проблему з датчиками температури усунено" : " не може бути активною через проблеми з датчиками температури");
    }
}

void MyHeatCustomFunctions::setFunctionAlert(byte functionIndex, FunctionAlert alert)
{
    FunctionAlert oldAlert = functionAlerts[functionIndex];
    if (oldAlert == alert)
    {
        return;
    }

    functionAlerts[functionIndex] = alert;

    if (oldAlert != FunctionAlert::FA_NONE)
    {
        MyHeatAlerts::resolveAlert(toAlertType(oldAlert), functionIndex, getFunctionAlertText(oldAlert, functionIndex, true));
    }
    if (alert != FunctionAlert::FA_NONE)
    {
        MyHeatAlerts::triggerAlert(toAlertType(alert), functionIndex, getFunctionAlertText(alert, functionIndex, false));
    }
}

void MyHeatCustomFunctions::resetFunctionAlert(byte functionIndex)
{
    if (functionIndex >= functionAlerts.size())
    {
        return;
    }

    // Reported as resolved so HA does not keep a stale problem, it is raised again if it still exists
    FunctionAlert oldAlert = functionAlerts[functionIndex];
    functionAlerts[functionIndex] = FunctionAlert::FA_NONE;

    if (oldAlert != FunctionAlert::FA_NONE)
    {
        MyHeatAlerts::resolveAlert(toAlertType(oldAlert), functionIndex, getFunctionAlertText(oldAlert, functionIndex, true));
    }
}

FunctionAlert MyHeatCustomFunctions::getFunctionAlert(byte functionIndex)
{
    return functionAlerts[functionIndex];
}

void MyHeatCustomFunctions::save()
{
    customFunctionsData->save();
}

float MyHeatCustomFunctions::getSensorValue(SourceType source, uint8_t index, EvalContext& ctx) {
    switch (source) {
        case SRC_TEMP:
            if (index >= ctx.temperatures.size()) {
                ctx.discoveredAlert = FunctionAlert::FA_BAD_TEMPERATURE_NUMBER;
                return NAN;
            }
            if (ctx.temperatures[index] <= TEMPERATURE_ERROR + 0.1) {
                ctx.discoveredAlert = FunctionAlert::FA_BAD_TEMPERATURE;
                return NAN;
            }
            return ctx.temperatures[index];
            
        case SRC_RELAY:
            if (index >= ctx.relays.size()) {
                ctx.discoveredAlert = FunctionAlert::FA_BAD_RELAY_NUMBER;
                return NAN;
            }
            return ctx.relays[index].getIsActive() ? 1.0f : 0.0f;
            
        case SRC_SMOKE:
            if (index == 0) return (float)ctx.smokeSensor.getValue();
            if (index == 1) return ctx.smokeSensor.getIsOverThreshold() ? 1.0f : 0.0f;
            return NAN;
            
        case SRC_SYSTEM:
            if (index == 0) return (float)(millis() / 1000.0f);   // uptime seconds
            if (index == 1) return (float)ESP.getFreeHeap();
            return NAN;
            
        default:
            return NAN;
    }
}

bool MyHeatCustomFunctions::evaluateCompare(ConditionNode& node, EvalContext& ctx) {
    float lhsBase = getSensorValue(node.source1, node.index1, ctx);
    
    // Availability checks
    if (node.op == OP_AVAILABLE)   return !std::isnan(lhsBase);
    if (node.op == OP_UNAVAILABLE) return std::isnan(lhsBase);
    
    float rhsBase = 0.0f;
    if (node.isValueSource) {
        rhsBase = getSensorValue(node.source2, node.index2, ctx);
    }
    
    // If either required sensor is invalid, fail-safe to false (prevent heater stuck ON)
    if (std::isnan(lhsBase) || (node.isValueSource && std::isnan(rhsBase))) {
        return false;
    }
    
    // Apply deltas
    float lhs = lhsBase + node.delta1;
    float rhs = node.isValueSource ? (rhsBase + node.delta2_or_value) : node.delta2_or_value;
    
    float hyst = node.hysteresis;
    bool wasTrue = (node.lastResult == 1);
    
    switch (node.op) {
        case OP_LT:
            if (wasTrue) return lhs < rhs + hyst;
            else         return lhs < rhs - hyst;
            
        case OP_GT:
            if (wasTrue) return lhs > rhs - hyst;
            else         return lhs > rhs + hyst;
            
        case OP_EQ:
            return std::abs(lhs - rhs) < (hyst > 0 ? hyst : 0.1f);

        case OP_NEQ:
            return std::abs(lhs - rhs) >= (hyst > 0 ? hyst : 0.1f);

        case OP_LTE:
            if (wasTrue) return lhs <= rhs + hyst;
            else         return lhs <= rhs - hyst;

        case OP_GTE:
            if (wasTrue) return lhs >= rhs - hyst;
            else         return lhs >= rhs + hyst;
            
        case OP_RANGE:
            return lhs >= node.delta2_or_value && lhs <= node.hysteresis;
            
        default: return false;
    }
}

bool MyHeatCustomFunctions::evaluateNode(int nodeIndex, std::vector<ConditionNode>& conditions, EvalContext& ctx) {
    if (nodeIndex < 0 || nodeIndex >= conditions.size()) return false;
    ConditionNode& node = conditions[nodeIndex];
    
    bool result = false;
    if (node.type == COND_CMP) {
        result = evaluateCompare(node, ctx);
    } else if (node.type == COND_AND) {
        result = true;
        for (int i = 0; i < node.childCount; i++) {
            if (!evaluateNode(node.childStart + i, conditions, ctx)) {
                result = false;
                break; // Short circuit
            }
        }
    } else if (node.type == COND_OR) {
        result = false;
        for (int i = 0; i < node.childCount; i++) {
            if (evaluateNode(node.childStart + i, conditions, ctx)) {
                result = true;
                break; // Short circuit
            }
        }
    } else if (node.type == COND_NOT) {
        if (node.childCount > 0) {
            result = !evaluateNode(node.childStart, conditions, ctx);
        }
    }
    
    node.lastResult = result ? 1 : 0;
    return result;
}

bool MyHeatCustomFunctions::evaluateFunction(MyHeatCustomFunction& func, EvalContext& ctx) {
    if (!func.getIsEnabled()) {
        func.setIsActive(false);
        return false;
    }
    
    auto& conditions = func.getConditions();
    if (conditions.empty()) {
        func.setIsActive(false);
        return false;
    }
    
    // Evaluate the root node (index 0)
    bool isActive = evaluateNode(0, conditions, ctx);
    func.setIsActive(isActive);
    return isActive;
}
