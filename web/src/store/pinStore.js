import { create } from 'zustand';

// Mock API response
const mockApiResponse = {
  usedPins: [
    { pin: 2 }, // relay 0
    { pin: 3 }, // relay 1
    { pin: 4 }, // relay 2
    { pin: 5 }, // relay 3
    { pin: 12 }, // sda
    { pin: 13 }, // scl
    { pin: 14 }, // encoder 0
    { pin: 15 }, // encoder 1
    { pin: 22 }, // encoder 2
    { pin: 21 }, // temperature sensor
  ]
};

// Pin configuration
const PIN_CONFIG = [
  { pin: 2, name: 'D2', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 3, name: 'D3', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 4, name: 'D4', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 5, name: 'D5', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 12, name: 'D12', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 13, name: 'D13', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 14, name: 'D14', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 15, name: 'D15', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 21, name: 'D21', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 22, name: 'D22', isInput: true, isOutput: true, isUsed: 0 },
  { pin: 23, name: 'D23', isInput: false, isOutput: true, isUsed: 0 },
  { pin: 25, name: 'D25', isInput: false, isOutput: true, isUsed: 0 },
  { pin: 26, name: 'D26', isInput: false, isOutput: true, isUsed: 0 },
  { pin: 27, name: 'D27', isInput: false, isOutput: true, isUsed: 0 },
  { pin: 32, name: 'D32', isInput: false, isOutput: true, isUsed: 0 },
  { pin: 33, name: 'D33', isInput: false, isOutput: true, isUsed: 0 },
];

const usePinStore = create((set, get) => ({
  pins: PIN_CONFIG,

  initializePins: async () => {
    const response = mockApiResponse;

    set(state => ({
      pins: state.pins.map(pin => ({
        ...pin,
        isUsed: response.usedPins.filter(usedPin => usedPin.pin === pin.pin).length
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

export default usePinStore;
