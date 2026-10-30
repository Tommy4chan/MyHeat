import {
  COND_AND, COND_NOT, COND_CMP,
  OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LTE, OP_GTE, OP_AVAILABLE, OP_UNAVAILABLE,
  SRC_TEMP, SRC_RELAY, SRC_SMOKE,
  ACT_SET_RELAY_TIMER,
} from "@/utils/functionEnums";
import { isGroup } from "./conditionTree";

export const OP_SYMBOLS = {
  [OP_LT]: "<", [OP_LTE]: "≤", [OP_GT]: ">", [OP_GTE]: "≥", [OP_EQ]: "=", [OP_NEQ]: "≠",
  [OP_AVAILABLE]: "доступний", [OP_UNAVAILABLE]: "недоступний",
};

// Relay state and smoke threshold are 0/1 values, compared only with = / ≠
export const isBooleanSource = (src, si) => src === SRC_RELAY || (src === SRC_SMOKE && si === 1);

export const getBooleanLabels = (src) =>
  src === SRC_RELAY ? { 1: "увімкнене", 0: "вимкнене" } : { 1: "перевищено", 0: "не перевищено" };

export const getUnit = (src) => (src === SRC_TEMP ? "°C" : "");

export const getSourceName = (src, si) => {
  switch (src) {
    case SRC_TEMP:  return `T${si}`;
    case SRC_RELAY: return `Реле ${si}`;
    case SRC_SMOKE: return si === 1 ? "Поріг диму" : "Дим";
    default:        return `Джерело ${src}:${si}`;
  }
};

const round = (value) => Math.round(value * 100) / 100;
const signed = (magnitude, sign) => Math.abs(magnitude ?? 0) * (sign ?? 1);
const formatDelta = (delta) => (delta ? ` ${delta > 0 ? "+" : "−"} ${Math.abs(delta)}` : "");

const describeSource = (src, si, delta) => getSourceName(src, si) + formatDelta(delta);

const describeRhs = (node) =>
  node.ivs
    ? describeSource(node.src2, node.si2 ?? 0, signed(node.d2, node.d2sign))
    : `${node.v ?? 0}${getUnit(node.src) ? ` ${getUnit(node.src)}` : ""}`;

export const describeCondition = (node) => {
  const lhs = describeSource(node.src, node.si ?? 0, signed(node.d1, node.d1sign));

  if (node.op === OP_AVAILABLE || node.op === OP_UNAVAILABLE) return `${lhs} ${OP_SYMBOLS[node.op]}`;

  if (isBooleanSource(node.src, node.si ?? 0) && !node.ivs) {
    const labels = getBooleanLabels(node.src);
    const state = (node.v ?? 1) === 1 ? 1 : 0;
    return `${lhs} ${node.op === OP_NEQ ? labels[1 - state] : labels[state]}`;
  }

  return `${lhs} ${OP_SYMBOLS[node.op] ?? "?"} ${describeRhs(node)}`;
};

export const describeTree = (node, isNested = false) => {
  if (!node) return "";
  if (node.t === COND_CMP) return describeCondition(node);
  if (node.t === COND_NOT) {
    const child = node.children?.[0];
    const text = describeTree(child, false);
    return isGroup(child) && child.children.length > 1 ? `не (${text})` : `не ${text}`;
  }

  const children = node.children || [];
  const text = children.map((child) => describeTree(child, true)).join(node.t === COND_AND ? " і " : " або ");
  return isNested && children.length > 1 ? `(${text})` : text;
};

const describeAction = (act) => {
  const text = `${act.v === 1 ? "увімкнути" : "вимкнути"} Реле ${act.ti}`;
  if (act.t !== ACT_SET_RELAY_TIMER) return text;
  act = { ...act, d: act.d ?? 60 };
  switch (act.tt) {
    case 1:  return `${text} на ${act.d} с`;
    case 2:  return `${text} циклами по ${act.d} с`;
    default: return `${text} (ще ${act.d} с після)`;
  }
};

export const describeFunction = (tree, actions) =>
  `Якщо ${describeTree(tree)} → ${(actions || []).map(describeAction).join(", ")}`;

// Thresholds follow evaluateCompare in MyHeatCustomFunctions.cpp: the condition turns on past v − h
// (or v + h) and stays on until the value crosses the opposite edge of the band
export const getHysteresisHint = (node) => {
  if (isBooleanSource(node.src, node.si ?? 0) && !node.ivs) return null;

  const hyst = Math.abs(node.hyst ?? 0);
  const unit = node.ivs ? "" : getUnit(node.src) ? ` ${getUnit(node.src)}` : "";

  if (node.op === OP_EQ || node.op === OP_NEQ) {
    const tolerance = hyst > 0 ? hyst : 0.1;
    return node.op === OP_EQ
      ? `рівне з допуском ±${tolerance}${unit}`
      : `відрізняється більш ніж на ${tolerance}${unit}`;
  }
  if (![OP_LT, OP_LTE, OP_GT, OP_GTE].includes(node.op) || hyst === 0) return null;

  const rhs = node.ivs ? describeRhs(node) : null;
  const low = rhs ? `${rhs} − ${hyst}` : `${round((node.v ?? 0) - hyst)}${unit}`;
  const high = rhs ? `${rhs} + ${hyst}` : `${round((node.v ?? 0) + hyst)}${unit}`;

  switch (node.op) {
    case OP_LT:  return `спрацює при < ${low}, відпустить при ≥ ${high}`;
    case OP_LTE: return `спрацює при ≤ ${low}, відпустить при > ${high}`;
    case OP_GT:  return `спрацює при > ${high}, відпустить при ≤ ${low}`;
    default:     return `спрацює при ≥ ${high}, відпустить при < ${low}`;
  }
};

// Timer behaviour follows updateRelays in MyHeatDevice.cpp
export const getActionHint = (act) => {
  const state = act.v === 1 ? "увімкненим" : "вимкненим";
  if (act.t !== ACT_SET_RELAY_TIMER) return `тримає ${state}, поки умова виконується`;
  act = { ...act, d: act.d ?? 60 };
  switch (act.tt) {
    case 1:  return `коли умова спрацює — ${state} на ${act.d} с, один раз`;
    case 2:  return `поки умова виконується: ${act.d} с ${state}, ${act.d} с пауза`;
    default: return `тримає ${state}, поки умова виконується, і ще ${act.d} с після`;
  }
};
