import { useEffect, useState } from "react";
import useSettingStore from "@/store/settingStore";

export const useMqttSettings = () => {
  const [settings, setSettings] = useState({
    broker: "",
    port: 1883,
    user: "",
    password: "",
    isEnabled: false,
    publishInterval: 60000,
  });

  const {
    setMqttSettings,
    getMqttSettings,
    mqttSettings,
  } = useSettingStore();

  useEffect(() => {
    getMqttSettings();
  }, []);

  useEffect(() => {
    setSettings(mqttSettings);
  }, [mqttSettings]);

  const handleSettingsChange = (e) => {
    const value = e.target.value;
    const name = e.target.name;
    setSettings((prev) => ({ ...prev, [name]: value }));
  };

  const handleMqttSettingsSave = () => {
    setMqttSettings(
      settings.broker,
      settings.port,
      settings.user,
      settings.password,
      settings.isEnabled,
      settings.publishInterval
    );
  };

  return {
    settings,
    handleSettingsChange,
    handleMqttSettingsSave,
  };
};
