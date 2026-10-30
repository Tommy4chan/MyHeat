import useFunctionStore from "@/store/functionStore";
import useTemperatureStore from "@/store/temperatureStore";
import useRelayStore from "@/store/relayStore";
import useSettingStore from "@/store/settingStore";
import { SRC_TEMP, SRC_RELAY, SRC_SMOKE, ACT_SET_RELAY, ACT_SET_RELAY_TIMER } from "@/utils/functionEnums";
import { getSourceName } from "../utils/describe";

// Values of the "src:si" select that merges source type and index
export const toSourceValue = (src, si) => `${src}:${si ?? 0}`;
export const fromSourceValue = (value) => value.split(":").map(Number);

// Timer select merges action type and timer type: "none" is a plain relay action
export const timerOptions = [
  { value: "none", text: "без таймера" },
  { value: "0",    text: "з затримкою вимкнення" },
  { value: "1",    text: "імпульсом" },
  { value: "2",    text: "циклічно" },
];
export const toTimerValue = (act) => (act.t === ACT_SET_RELAY_TIMER ? String(act.tt ?? 0) : "none");
export const fromTimerValue = (value) =>
  value === "none" ? { t: ACT_SET_RELAY } : { t: ACT_SET_RELAY_TIMER, tt: +value };

export const useFormOptions = () => {
  const { temperatureCount, relayCount } = useFunctionStore();
  const { temperatures } = useTemperatureStore();
  const { relays } = useRelayStore();
  const { smokeSensor } = useSettingStore();

  const sourceOptions = [
    ...Array.from({ length: temperatureCount }, (_, i) => ({ value: toSourceValue(SRC_TEMP, i), text: getSourceName(SRC_TEMP, i) })),
    ...Array.from({ length: relayCount }, (_, i) => ({ value: toSourceValue(SRC_RELAY, i), text: getSourceName(SRC_RELAY, i) })),
    { value: toSourceValue(SRC_SMOKE, 0), text: "Дим: значення" },
    { value: toSourceValue(SRC_SMOKE, 1), text: "Дим: поріг" },
  ];

  // Keeps a saved source selectable even if that sensor or relay no longer exists
  const getSourceOptions = (src, si) => {
    const value = toSourceValue(src, si);
    return sourceOptions.some((o) => o.value === value)
      ? sourceOptions
      : [{ value, text: `${getSourceName(src, si ?? 0)} (немає)` }, ...sourceOptions];
  };

  const getRelayOptions = (ti) => {
    const options = Array.from({ length: relayCount }, (_, i) => ({ value: i, text: getSourceName(SRC_RELAY, i) }));
    return ti < relayCount ? options : [{ value: ti, text: `${getSourceName(SRC_RELAY, ti)} (немає)` }, ...options];
  };

  // Current reading of a source, only to help the user pick thresholds
  const getLiveValue = (src, si) => {
    switch (src) {
      case SRC_TEMP: {
        const value = parseFloat(temperatures[si]?.value);
        if (isNaN(value)) return null;
        return value <= -127 ? "помилка" : `${value.toFixed(1)} °C`;
      }
      case SRC_RELAY: {
        const relay = relays[si];
        return relay ? (relay.isActive ? "увімкнене" : "вимкнене") : null;
      }
      case SRC_SMOKE: {
        if (si === 1) return smokeSensor?.isOverThreshold === undefined ? null : smokeSensor.isOverThreshold ? "перевищено" : "не перевищено";
        if (smokeSensor?.value === undefined) return null;
        return smokeSensor.value === -1 ? "ініціалізація" : String(smokeSensor.value);
      }
      default:
        return null;
    }
  };

  return { getSourceOptions, getRelayOptions, getLiveValue };
};
