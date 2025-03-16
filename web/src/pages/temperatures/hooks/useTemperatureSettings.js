import { useState, useEffect } from "react";
import useTemperatureStore from "@/store/temperatureStore";
import usePinStore from "@/store/pinStore";

export const useTemperatureSettings = () => {
  const {
    temperatureSettings,
    temperatureSensorsCountCalculated,
    getTemperatureSensorsSettings,
    setTemperatureSensorsSettings,
  } = useTemperatureStore();

  const { handlePinChange, getPins } = usePinStore();

  const [settings, setSettings] = useState({
    temperaturePin: 0,
    temperatureCount: 1,
    minTemperature: 0,
    maxTemperature: 0,
  });

  useEffect(() => {
    getTemperatureSensorsSettings();
    getPins();
  }, [temperatureSensorsCountCalculated]);

  useEffect(() => {
    setSettings({
      temperaturePin: temperatureSettings.temperaturePin,
      temperatureCount: temperatureSettings.temperatureCount,
      minTemperature: temperatureSettings.minTemperature,
      maxTemperature: temperatureSettings.maxTemperature,
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

  const handleSettingsSave = () => {
    setTemperatureSensorsSettings(
      settings.temperaturePin,
      settings.temperatureCount,
      settings.minTemperature,
      settings.maxTemperature
    );
  };

  return {
    settings,
    handleSettingsChange,
    handleSettingsSave,
  };
};
