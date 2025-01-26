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

  processGetWifiSettings: (data) => {
    if (!data?.payload) return;

    set({ wifiSettings: data.payload });
  },

  processWifiScanData: (data) => {
    set({ isScanningForWifiNetworks: false });

    if (!data?.payload) return;

    set({ scannedWifiNetworks: data.payload });
  },
}));

useWebSocketStore.subscribe(
  (state) => state.messages["getWifiSettingsResponse"],
  (message) => {
    if (message) {
      useSettingStore.getState().processGetWifiSettings(message);
    }
  }
);

useWebSocketStore.subscribe(
  (state) => state.messages["wifiScanData"],
  (message) => {
    if (message) {
      useSettingStore.getState().processWifiScanData(message);
    }
  }
);

export default useSettingStore;
