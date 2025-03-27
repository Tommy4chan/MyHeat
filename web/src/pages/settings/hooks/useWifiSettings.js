import { useEffect, useState } from "react";
import useSettingStore from "@/store/settingStore";

export const useWifiSettings = () => {
  const [settings, setSettings] = useState({
    wifiSSID: "",
    wifiPassword: "",
    apSSID: "",
    apPassword: "",
    isFallbackAPEnabled: false,
    mDNS: "",
  });

  const [networks, setNetworks] = useState([]);

  const {
    setWifiSettings,
    getWifiSettings,
    startWifiScan,
    wifiSettings,
    scannedWifiNetworks,
    isScanningForWifiNetworks,
  } = useSettingStore();

  useEffect(() => {
    getWifiSettings();
  }, []);

  useEffect(() => {
    setSettings(wifiSettings);
  }, [wifiSettings]);

  useEffect(() => {
    setNetworks(scannedWifiNetworks);
  }, [scannedWifiNetworks]);

  const handleSettingsChange = (e) => {
    const value = e.target.value;
    const name = e.target.name;

    if (name === "pin") {
      handleSensorPinChange(e);
    } else {
      setSettings((prev) => ({ ...prev, [name]: value }));
    }
  };

  const handleWifiSettingsSave = () => {
    setWifiSettings(settings.wifiSSID, settings.wifiPassword, settings.apSSID, settings.apPassword, settings.isFallbackAPEnabled, settings.mDNS);
  };

  return {
    settings,
    networks,
    handleSettingsChange,
    handleWifiSettingsSave,
    startWifiScan,
    isScanningForWifiNetworks,
  };
};
