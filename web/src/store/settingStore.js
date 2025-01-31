import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useSettingStore = create((set) => ({
  wifiSettings: {},
  scannedWifiNetworks: [],
  isScanningForWifiNetworks: false,

  setWifiCredentials: (ssid, password) => {
    const payload = {
      ssid, 
      password,
    };
    useWebSocketStore.getState().sendMessage("setWifiSettings", payload);
  },

  startWifiScan: () => {
    set({ scannedWifiNetworks: [], isScanningForWifiNetworks: true });
    useWebSocketStore.getState().sendMessage("startWifiScan");
  },

  getWifiSettings: () => {
    useWebSocketStore.getState().sendMessage("getWifiSettings");
  },

  processGetWifiSettings: (payload) => {
    set({ wifiSettings: payload });
  },

  processWifiScanData: (payload) => {
    set({ isScanningForWifiNetworks: false, scannedWifiNetworks: payload });
  },
}));

useWebSocketStore.subscribe(
  (state) => state.messages["getWifiSettingsResponse"],
  (payload) => useSettingStore.getState().processGetWifiSettings(payload),
);

useWebSocketStore.subscribe(
  (state) => state.messages["wifiScanData"],
  (payload) => useSettingStore.getState().processWifiScanData(payload),
);

export default useSettingStore;
