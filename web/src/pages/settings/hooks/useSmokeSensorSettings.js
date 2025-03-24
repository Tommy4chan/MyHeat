import { useEffect, useState } from "react";
import useSettingStore from "@/store/settingStore";
import usePinStore from "../../../store/pinStore";

export const useSmokeSensorSettings = () => {
  const { handlePinChange } = usePinStore();

  const {
    smokeSensorSettings,
    smokeSensor,
    getSmokeSensor,
    setSmokeSensorSettings,
    getSmokeSensorSettings,
  } = useSettingStore();

  const [sensor, setSensor] = useState({
    value: 0,
    isOverThreshold: false,
  });

  const [settings, setSettings] = useState({
    isEnabled: false,
    threshold: 0,
    pin: 0,
  });

  useEffect(() => {
    getSmokeSensorSettings();
    getSmokeSensor();

    const interval = setInterval(() => {
      getSmokeSensor();
    }, 2000);

    return () => clearInterval(interval);
  }, []);

  useEffect(() => {
    setSensor(smokeSensor);
  }, [smokeSensor]);

  useEffect(() => {
    setSettings(smokeSensorSettings);
  }, [smokeSensorSettings]);

  const handleSensorPinChange = (value) => {
    const newPin = parseInt(value);
    const oldPin = parseInt(settings.pin);

    const success = handlePinChange(oldPin, newPin);

    if (success) {
      setSettings((prev) => ({ ...prev, pin: newPin }));
    }
  };

  const handleSettingsChange = (e) => {
    const value = e.target.value;
    const name = e.target.name;

    if (name === "pin") {
      handleSensorPinChange(value);
    } else {
      setSettings((prev) => ({ ...prev, [name]: value }));
    }
  };

  const handleSmokeSensorSettingsSave = () => {
    setSmokeSensorSettings(
      settings.isEnabled,
      settings.threshold,
      settings.pin
    );
  };

  return {
    settings,
    sensor,
    handleSettingsChange,
    handleSmokeSensorSettingsSave,
  };
};
