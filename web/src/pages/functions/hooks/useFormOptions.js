import useFunctionStore from "@/store/functionStore";
import {
  COND_AND, COND_OR, COND_NOT, COND_CMP,
  OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LTE, OP_GTE, OP_AVAILABLE, OP_UNAVAILABLE,
  SRC_TEMP, SRC_RELAY, SRC_SMOKE,
  ACT_SET_RELAY, ACT_SET_RELAY_TIMER,
} from "@/utils/functionEnums";

// Operators that have a right-hand side value/source
export const OPS_WITH_RHS = new Set([OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LTE, OP_GTE]);

// Operators that support hysteresis
export const OPS_WITH_HYST = new Set([OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LTE, OP_GTE]);

export const useFormOptions = () => {
  const { temperatureCount, relayCount } = useFunctionStore();

  const tempSensorsOptions = Array.from({ length: temperatureCount }, (_, i) => ({ value: i, text: `T${i}` }));

  const relayOptions = Array.from({ length: relayCount }, (_, i) => ({
    value: i,
    text: `Реле ${i}`,
  }));

  // Source type options — what the LHS and RHS2 can be
  const sourceTypeOptions = [
    { value: SRC_TEMP,  text: "Датчик T" },
    { value: SRC_RELAY, text: "Реле" },
    { value: SRC_SMOKE, text: "Датчик диму" },
  ];

  // Returns the index sub-options depending on chosen source type
  const getIndexOptions = (sourceType) => {
    switch (sourceType) {
      case SRC_TEMP:  return tempSensorsOptions;
      case SRC_RELAY: return relayOptions;
      case SRC_SMOKE: return [
        { value: 0, text: "Значення" },
        { value: 1, text: "Перевищення порогу" },
      ];
      default: return [];
    }
  };

  const conditionTypeOptions = [
    { value: COND_CMP, text: "Порівняння" },
    { value: COND_AND, text: "І (AND)" },
    { value: COND_OR,  text: "АБО (OR)" },
    { value: COND_NOT, text: "НІ (NOT)" },
  ];

  const cmpOpOptions = [
    { value: OP_LT,          text: "<" },
    { value: OP_LTE,         text: "≤" },
    { value: OP_GT,          text: ">" },
    { value: OP_GTE,         text: "≥" },
    { value: OP_EQ,          text: "=" },
    { value: OP_NEQ,         text: "≠" },
    { value: OP_AVAILABLE,   text: "Доступний" },
    { value: OP_UNAVAILABLE, text: "Недоступний" },
  ];

  const actionTypeOptions = [
    { value: ACT_SET_RELAY,       text: "Увімк. / Вимк. реле" },
    { value: ACT_SET_RELAY_TIMER, text: "Реле з таймером" },
  ];

  const relayStateOptions = [
    { value: 1, text: "Увімкнути" },
    { value: 0, text: "Вимкнути" },
  ];

  const timerTypeOptions = [
    { value: 0, text: "Перезапуск (Off-Delay)" },
    { value: 1, text: "Імпульс (One-Shot)" },
    { value: 2, text: "Циклічний (Extend)" },
  ];

  return {
    tempSensorsOptions,
    relayOptions,
    sourceTypeOptions,
    getIndexOptions,
    conditionTypeOptions,
    cmpOpOptions,
    actionTypeOptions,
    relayStateOptions,
    timerTypeOptions,
  };
};
