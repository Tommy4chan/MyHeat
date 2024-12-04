import usePinStore from "../store/pinStore";

/**
 * Generic handler for pin changes.
 * @param {Event} e - Event object from the input.
 * @param {function} setState - State setter function from useState.
 * @param {Array} currentPins - Current pins array from state.
 * @param {number} pinIndex - Index of the pin being changed.
 */
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
