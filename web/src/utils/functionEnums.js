// ConditionType — matches C++ enum ConditionType in MyHeatCustomFunction.h
export const COND_AND = 0;
export const COND_OR  = 1;
export const COND_NOT = 2;
export const COND_CMP = 3;

// CompareOp — matches C++ enum CompareOp
export const OP_EQ          = 0;  // ==
export const OP_NEQ         = 1;  // !=
export const OP_LT          = 2;  // <
export const OP_GT          = 3;  // >
export const OP_LTE         = 4;  // <=
export const OP_GTE         = 5;  // >=
export const OP_RANGE       = 6;  // min <= x <= max
export const OP_AVAILABLE   = 7;  // sensor is valid (not NaN)
export const OP_UNAVAILABLE = 8;  // sensor is invalid (NaN)

// SourceType — matches C++ enum SourceType
export const SRC_TEMP   = 0;
export const SRC_RELAY  = 1;
export const SRC_SMOKE  = 2;
export const SRC_ANALOG = 3;
export const SRC_SYSTEM = 4;

// ActionType — matches C++ enum ActionType
export const ACT_SET_RELAY       = 0;
export const ACT_SET_RELAY_TIMER = 1;

// TimerType — matches C++ implementation
export const TIMER_TYPE_RESTART = 0;
export const TIMER_TYPE_PULSE   = 1;
export const TIMER_TYPE_EXTEND  = 2;

// Convenience
export const TEMP_UNKNOWN  = 255;
export const RELAY_UNKNOWN = 255;
