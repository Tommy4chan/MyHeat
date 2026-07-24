#ifndef MYHEATCUSTOMFUNCTION_H
#define MYHEATCUSTOMFUNCTION_H

#include <Arduino.h>
#include <vector>
#include "MyHeatEnums.h"

enum ConditionType : uint8_t {
    COND_AND = 0,
    COND_OR  = 1,
    COND_NOT = 2,
    COND_CMP = 3
};

enum CompareOp : uint8_t {
    OP_EQ  = 0,    // ==
    OP_NEQ = 1,    // !=
    OP_LT  = 2,    // <
    OP_GT  = 3,    // >
    OP_LTE = 4,    // <=
    OP_GTE = 5,    // >=
    OP_RANGE = 6,  // value <= x <= value2
    OP_AVAILABLE   = 7,   // sensor is valid
    OP_UNAVAILABLE = 8    // sensor is invalid
};

enum SourceType : uint8_t {
    SRC_TEMP   = 0,
    SRC_RELAY  = 1,
    SRC_SMOKE  = 2,
    SRC_ANALOG = 3,
    SRC_SYSTEM = 4
};

enum ActionType : uint8_t {
    ACT_SET_RELAY       = 0,   // set relay ON/OFF directly
    ACT_SET_RELAY_TIMER = 1    // set relay ON/OFF for N seconds
};

struct ConditionNode {
    ConditionType type;         // 1B
    
    // Left-hand side (LHS)
    SourceType source1;         // 1B
    uint8_t index1;             // 1B
    
    CompareOp op;               // 1B
    
    // Right-hand side (RHS)
    bool isValueSource;         // 1B — false = RHS is constant, true = RHS is source2
    SourceType source2;         // 1B — only used if isValueSource
    uint8_t index2;             // 1B — only used if isValueSource
    
    uint8_t childCount;         // 1B — for AND/OR/NOT
    uint8_t childStart;         // 1B — index into conditions array
    int8_t lastResult;          // 1B — -1=never eval, 0=false, 1=true
    
    // Values
    float delta1;               // 4B — added to LHS (e.g. temp1 + delta1)
    float delta2_or_value;      // 4B — added to RHS (if isValueSource) OR constant target value
    float hysteresis;           // 4B — hysteresis (or range max if OP_RANGE)
};

struct ActionNode {
    ActionType type;            // 1B
    uint8_t targetIndex;        // 1B — relay index
    uint8_t value;              // 1B — 0=OFF, 1=ON
    uint32_t duration;          // 4B — seconds (for timer actions, 0 = indefinite)
    uint8_t timerType;          // 1B — 0=Restart, 1=Pulse, 2=Extend
    bool isTimerRunning;        // 1B
    bool prevConditionState;    // 1B
    uint32_t timerStartMillis;  // 4B
};

class MyHeatCustomFunction
{
private:
    bool isEnabled;
    bool isActive;
    uint8_t priority;
    char name[24];

    std::vector<ConditionNode> conditions;
    std::vector<ActionNode> actions;

public:
    MyHeatCustomFunction()
    {
        this->isEnabled = false;
        this->isActive = false;
        this->priority = 0;
        memset(this->name, 0, sizeof(this->name));
    }

    bool getIsEnabled() const { return this->isEnabled; }
    void setIsEnabled(bool isEnabled) { this->isEnabled = isEnabled; }
    void toggleIsEnabled() { this->isEnabled = !this->isEnabled; }

    bool getIsActive() const { return this->isActive; }
    void setIsActive(bool isActive) { this->isActive = isActive; }

    uint8_t getPriority() const { return this->priority; }
    void setPriority(uint8_t priority) { this->priority = priority; }

    const char* getName() const { return this->name; }
    void setName(const char* name) { strncpy(this->name, name, sizeof(this->name) - 1); }

    std::vector<ConditionNode>& getConditions() { return this->conditions; }
    std::vector<ActionNode>& getActions() { return this->actions; }
    const std::vector<ConditionNode>& getConditions() const { return this->conditions; }
    const std::vector<ActionNode>& getActions() const { return this->actions; }

    bool isValid() const { return true; }
};

#endif
