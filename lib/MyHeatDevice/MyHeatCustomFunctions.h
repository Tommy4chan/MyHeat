#ifndef MYHEATCUSTOMFUNCTIONS_H
#define MYHEATCUSTOMFUNCTIONS_H

#include <Arduino.h>
#include <vector>
#include <LittleFS.h>
#include "MyHeatCustomFunction.h"
#include "MyHeatSave.h"

// Forward declarations to avoid heavy includes if possible
class MyHeatRelay;
class MyHeatSmokeSensor;

enum FunctionAlert
{
    FA_NONE = 0,
    FA_BAD_TEMPERATURE = 1,
    FA_BAD_TEMPERATURE_NUMBER = 2,
    FA_BAD_RELAY_NUMBER = 3,
};

struct EvalContext {
    std::vector<float>& temperatures;
    std::vector<MyHeatRelay>& relays;
    MyHeatSmokeSensor& smokeSensor;
    byte currentFunctionIndex = 0;
    FunctionAlert discoveredAlert = FA_NONE;
};

class MyHeatCustomFunctions : public MyHeatSaveInterface
{
private:
    std::vector<MyHeatCustomFunction> customFunctions;
    MyHeatSave *customFunctionsData;
    std::vector<FunctionAlert> functionAlerts;

    void serialize(JsonDocument &doc) override;
    void deserialize(const JsonDocument &doc) override;
    void realocateMemory(byte newFunctionCount);

public:
    MyHeatCustomFunctions();
    void begin();
    
    MyHeatCustomFunction& getCustomFunction(byte index);
    std::vector<MyHeatCustomFunction>& getCustomFunctions();
    
    void toggleCustomFunctionIsEnabled(byte functionIndex);
    bool setCustomFunctionIsEnabled(byte functionIndex, bool isEnabled);
    
    void setCustomFunctionCount(byte newCustomFunctionCount);
    byte getCustomFunctionCount();
    
    void manualDeserialize(const JsonDocument& payload);
    
    void setFunctionAlert(byte functionIndex, FunctionAlert alert);
    void resetFunctionAlert(byte functionIndex);
    FunctionAlert getFunctionAlert(byte functionIndex);
    
    void save();
    
    // Evaluation engine
    float getSensorValue(SourceType source, uint8_t index, EvalContext& ctx);
    bool evaluateCompare(ConditionNode& node, EvalContext& ctx);
    bool evaluateNode(int nodeIndex, std::vector<ConditionNode>& conditions, EvalContext& ctx);
    bool evaluateFunction(MyHeatCustomFunction& func, EvalContext& ctx);
};

#endif
