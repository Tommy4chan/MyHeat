import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useSettingStore = create((set) => ({
  wifiSettings: {},
  scannedWifiNetworks: [],

  setWifiCredentials: (ssid, password) => {
    const message = {
      messageType: "setWifi",
      payload: { ssid, password },
    };
    useWebSocketStore.getState().sendMessage("setWifiCredentials", message);
  },

  startWifiScan: () => {
    set({ scannedWifiNetworks: [] });
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
