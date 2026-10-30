import { create } from 'zustand';
import useWebSocketStore from "./websocketStore";

// Pin 0 is the "not assigned" entry, it is shown but can't be selected
const NOT_ASSIGNED = { pin: 0, name: 'Н/Д', isInput: true, isOutput: true, isAnalog: false, isUsed: 0 };

const pin = (number, prefix, { isInput = true, isOutput = true, isAnalog = false } = {}) =>
  ({ pin: number, name: `${prefix}${number}`, isInput, isOutput, isAnalog, isUsed: 0 });

const range = (from, to) => Array.from({ length: to - from + 1 }, (_, i) => from + i);

// Classic ESP32 (ESP32-DevKit / NodeMCU-32S). 34-39 are input only, ADC1 is 32-39 (ADC2 stops working with WiFi)
const ESP32_PINS = [
  NOT_ASSIGNED,
  ...[2, 4, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27].map((n) => pin(n, 'D')),
  pin(12, 'D', { isInput: false }),
  ...[32, 33].map((n) => pin(n, 'D', { isAnalog: true })),
  ...[34, 35, 36, 39].map((n) => pin(n, 'D', { isOutput: false, isAnalog: true })),
].sort((a, b) => a.pin - b.pin);

// ESP32-S3 (DevKitC-1). Left out: 19/20 USB, 26-32 SPI flash, 43/44 UART0 (USB-UART bridge),
// 45/46 strapping pins that break booting when pulled. ADC1 is 1-10
const ESP32S3_PINS = [
  NOT_ASSIGNED,
  ...range(1, 10).map((n) => pin(n, 'GPIO', { isAnalog: true })),
  ...[...range(11, 18), 21, ...range(33, 42), 47, 48].map((n) => pin(n, 'GPIO')),
];

const PIN_CONFIGS = {
  esp32: ESP32_PINS,
  esp32s3: ESP32S3_PINS,
};

const getPinText = (pin, selectedPin) =>
  pin.name + ((pin.isUsed == 1 && pin.pin !== selectedPin) ? ' 🔧' : pin.isUsed >= 2 ? '❗' : '');

// Keeps a saved pin selectable even when it is not in the list for this chip or purpose
const toOptions = (pins, allPins, selectedPin) => {
  const options = pins.map((pin) => ({ value: pin.pin, text: getPinText(pin, selectedPin), isDisabled: pin.pin === 0 }));
  if (selectedPin !== undefined && selectedPin !== null && !pins.some((pin) => pin.pin === selectedPin)) {
    const known = allPins.find((pin) => pin.pin === selectedPin);
    options.unshift({ value: selectedPin, text: `${known ? known.name : `GPIO${selectedPin}`} (не підходить)` });
  }
  return options;
};

const usePinStore = create((set, get) => ({
  pins: ESP32_PINS,

  getPins: () => {
    useWebSocketStore.getState().sendMessage("getPinsData");
  },

  proccessPins: (payload) => {
    const config = PIN_CONFIGS[payload.chip] ?? ESP32_PINS;
    set(() => ({
      pins: config.map(pin => ({
        ...pin,
        isUsed: payload.usedPins.filter(usedPin => usedPin === pin.pin).length
      }))
    }));
  },

  markPinAsUsed: (pinNumber) => set(state => ({
    pins: state.pins.map(pin => 
      pin.pin === pinNumber ? { ...pin, isUsed: pin.isUsed + 1 } : pin
    )
  })),

  markPinAsUnused: (pinNumber) => set(state => ({
    pins: state.pins.map(pin => 
      pin.pin === pinNumber 
        ? { ...pin, isUsed: pin.isUsed > 0 ? pin.isUsed - 1 : 0 } 
        : pin
    )
  })),

  handlePinChange: (oldPin, newPin) => {
    const state = get();

    if (oldPin !== null && oldPin !== undefined) {
      const oldPinConfig = state.pins.find(p => p.pin === oldPin);
      if (oldPinConfig && oldPinConfig.isUsed > 0) {
        get().markPinAsUnused(oldPin);
      }
    }

    if (newPin !== null && newPin !== undefined) {
      const newPinConfig = state.pins.find(p => p.pin === newPin);

      if (!newPinConfig) {
        console.error(`Pin ${newPin} does not exist`);
        return false;
      }

      get().markPinAsUsed(newPin);
      return true;
    }

    return true;
  },

  getAvailableInputPins: (selectedPin) =>
    toOptions(get().pins.filter(pin => pin.isInput), get().pins, selectedPin),

  getAvailableOutputPins: (selectedPin) =>
    toOptions(get().pins.filter(pin => pin.isOutput), get().pins, selectedPin),

  // OneWire needs a pin that can both read and drive the bus
  getAvailableIOPins: (selectedPin) =>
    toOptions(get().pins.filter(pin => pin.isInput && pin.isOutput), get().pins, selectedPin),

  // ADC1 only, ADC2 can't be read while WiFi is on
  getAvailableAnalogPins: (selectedPin) =>
    toOptions(get().pins.filter(pin => pin.isAnalog || pin.pin === 0), get().pins, selectedPin),
}));

useWebSocketStore.subscribe(
  (state) => state.messages["getPinsDataResponse"],
  (payload) => usePinStore.getState().proccessPins(payload),
);

export default usePinStore;
