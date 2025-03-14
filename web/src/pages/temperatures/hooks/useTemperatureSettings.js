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
    minTemperatureNotification: 5,
    maxTemperatureNotification: 90,
  });

  useEffect(() => {
    getTemperatureSensorsSettings();
    getPins();
  }, [temperatureSensorsCountCalculated]);

  useEffect(() => {
    setSettings({
      temperaturePin: temperatureSettings.temperaturePin,
      temperatureCount: temperatureSettings.temperatureCount,
      minTemperatureNotification: 5,
      maxTemperatureNotification: 90,
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
      settings.temperatureCount
    );
  };

  return {
    settings,
    handleSettingsChange,
    handleSettingsSave,
  };
};
