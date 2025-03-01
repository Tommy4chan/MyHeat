import { useEffect, useState } from "react";
import useSettingStore from "@/store/settingStore";
import usePinStore from "../../../store/pinStore";

export const useHardwareSettings = () => {
  const { handlePinChange, getPins } = usePinStore();

  const { getHardwareIOSettings, hardwareIOSettings, setHardwareIOSettings } =
    useSettingStore();

  const [isActive, setIsActive] = useState(true);

  const [oledAddress, setOledAddress] = useState("");
  const [screenPowerSaveInterval, setScreenPowerSaveInterval] = useState(0);
  const [oledPins, setOledPins] = useState([0, 0]);

  const [encInvert, setEncInvert] = useState(false);
  const [encoderPins, setEncoderPins] = useState([0, 0, 0]);

  const handleHardwarePinChange = (pinIndex, oldPins, setPins) => (e) => {
    const newPin = parseInt(e.target.value);
    const oldPin = parseInt(oldPins[pinIndex]);

    const success = handlePinChange(oldPin, newPin);

    if (success) {
      const newPins = [...oldPins];
      newPins[pinIndex] = newPin;
      setPins(newPins);
    }
  };

  useEffect(() => {
    getPins();
    getHardwareIOSettings();
  }, []);

  useEffect(() => {
    setIsActive(hardwareIOSettings.isActive);
    setOledAddress(hardwareIOSettings.oledAddress);
    setScreenPowerSaveInterval(hardwareIOSettings.screenPowerSaveInterval);
    setOledPins([hardwareIOSettings.oledSDA, hardwareIOSettings.oledSCL]);
    setEncInvert(hardwareIOSettings.encInvert);
    setEncoderPins([
      hardwareIOSettings.encA,
      hardwareIOSettings.encB,
      hardwareIOSettings.encBtn,
    ]);
  }, [hardwareIOSettings]);

  const handleSaveHardwareIOSettings = () => {
    setHardwareIOSettings(
      isActive,
      oledAddress,
      screenPowerSaveInterval,
      oledPins,
      encInvert,
      encoderPins
    );
  };

  return {
    isActive,
    setIsActive,
    oledAddress,
    setOledAddress,
    screenPowerSaveInterval,
    setScreenPowerSaveInterval,
    encInvert,
    setEncInvert,
    encoderPins,
    setEncoderPins,
    oledPins,
    setOledPins,
    handleHardwarePinChange,
    handleSaveHardwareIOSettings,
  };
};
