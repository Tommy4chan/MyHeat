import { create } from 'zustand';
import useWebSocketStore from "./websocketStore";

const PIN_CONFIG = [
  { pin: 2, name: 'D2', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 4, name: 'D4', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 12, name: 'D12', isInput: false, isOutput: true, isUsed: 0 },
  { pin: 13, name: 'D13', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 14, name: 'D14', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 15, name: 'D15', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 16, name: 'D16', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 17, name: 'D17', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 18, name: 'D18', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 19, name: 'D19', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 21, name: 'D21', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 22, name: 'D22', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 23, name: 'D23', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 25, name: 'D25', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 26, name: 'D26', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 27, name: 'D27', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 32, name: 'D32', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 33, name: 'D33', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 34, name: 'D34', isInput: true, isOutput: false, isUsed: 0 },
  { pin: 35, name: 'D35', isInput: true, isOutput: false, isUsed: 0 },
  { pin: 36, name: 'D36', isInput: true, isOutput: false, isUsed: 0 },
  { pin: 39, name: 'D39', isInput: true, isOutput: false, isUsed: 0 },
];

const usePinStore = create((set, get) => ({
  pins: PIN_CONFIG,

  getPins: () => {
    useWebSocketStore.getState().sendMessage("getPinsData");
  },

  proccessPins: (data) => {
    if (!data?.payload) return;

    const response = data.payload.usedPins;

    set(state => ({
      pins: state.pins.map(pin => ({
        ...pin,
        isUsed: response.filter(usedPin => usedPin === pin.pin).length
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
    get().pins
      .filter(pin => pin.isInput)
      .map(pin => ({ value: pin.pin, text: pin.name + ((pin.isUsed == 1 && pin.pin !== selectedPin) ? ' 🔧' : pin.isUsed >= 2 ? '❗' : '') })),

  getAvailableOutputPins: (selectedPin) => 
    get().pins
      .filter(pin => pin.isOutput)
      .map(pin => ({ value: pin.pin, text: pin.name + ((pin.isUsed == 1 && pin.pin !== selectedPin) ? ' 🔧' : pin.isUsed >= 2 ? '❗' : '') })),
}));

useWebSocketStore.subscribe(
  (state) => state.messages["getPinsDataResponse"],
  (message) => {
    if (message) {
      usePinStore.getState().proccessPins(message);
    }
  }
);

export default usePinStore;
