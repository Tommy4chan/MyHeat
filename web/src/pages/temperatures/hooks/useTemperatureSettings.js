import { useState, useEffect, useCallback } from "react";
import useTemperatureStore from "@/store/temperatureStore";
import usePinStore from "@/store/pinStore";

export const useTemperatureSettings = () => {
  const {
    temperatureSettings,
    getTemperatureSensorsSettings,
    setTemperatureSensorsSettings,
  } = useTemperatureStore();

  const { handlePinChange } = usePinStore();

  const [settings, setSettings] = useState({
    temperaturePin: 0,
    temperatureCount: 1,
  });

  useEffect(() => {
    getTemperatureSensorsSettings();
  }, []);

  useEffect(() => {
    setSettings({
      temperaturePin: temperatureSettings.temperaturePin,
      temperatureCount: temperatureSettings.temperatureCount,
    });
  }, [temperatureSettings]);

  const handleSettingsChange = (e) => {
    const name = e.target.name;

    if (name === "temperaturePin") {
      const newPin = parseInt(e.target.value);
      const oldPin = parseInt(settings.temperaturePin);

      const success = handlePinChange(oldPin, newPin);

      if (success) {
        setSettings((prev) => ({ ...prev, temperaturePin: newPin }));
      }
    } else {
      setSettings((prev) => ({ ...prev, [name]: e.target.value }));
    }
  };

  const handleSettingsSave = useCallback(() => {
    setTemperatureSensorsSettings(
      settings.temperaturePin,
      settings.temperatureCount
    );
  }, [settings, setTemperatureSensorsSettings]);

  return {
    settings,
    handleSettingsChange,
    handleSettingsSave,
  };
};
