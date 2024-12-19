import usePinStore from "../store/pinStore";

export const handlePinChange = (e, setState, currentPins, pinIndex = null) => {
  const newPin = parseInt(e.target.value);
  const oldPin = Array.isArray(currentPins)
    ? currentPins[pinIndex]
    : currentPins;

  const { handlePinChange } = usePinStore.getState();
  const success = handlePinChange(oldPin, newPin);

  if (success) {
    if (Array.isArray(currentPins)) {
      const updatedPins = currentPins.map((pin, index) =>
        index === pinIndex ? newPin : pin
      );
      setState(updatedPins);
    } else {
      setState(newPin);
    }
  }
};
