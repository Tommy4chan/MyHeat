import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useSettingStore = create((set) => ({
  wifiSettings: {},
  scannedWifiNetworks: [],
  isScanningForWifiNetworks: false,
  telegramBotSettings: {},
  ntpSettings: {},

  setWifiSettings: (ssid, password, mDNS) => {
    const payload = {
      ssid, 
      password,
      mDNS,
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

  setTelegramBotSettings: (token, registerPhrase, isActive) => {
    isActive = isActive === "true" ? true : false;

    const payload = {
      token,
      registerPhrase,
      isActive
    };

    useWebSocketStore.getState().sendMessage("setTelegramBotSettings", payload);
  },

  getTelegramBotSettings: () => {
    useWebSocketStore.getState().sendMessage("getTelegramBotSettings");
  },

  processGetTelegramBotSettings: (payload) => {
    set({ telegramBotSettings: payload });
  },

  setNtpSettings: (ntpServer, ntpIANA, ntpOffset, ntpDaylightOffset) => {
    const payload = {
      ntpServer,
      ntpIANA,
      ntpOffset,
      ntpDaylightOffset,
    };

    useWebSocketStore.getState().sendMessage("setNTPSettings", payload);
  },

  getNtpSettings: () => {
    useWebSocketStore.getState().sendMessage("getNTPSettings");
  },

  processGetNtpSettings: (payload) => {
    set({ ntpSettings: payload });
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

useWebSocketStore.subscribe(
  (state) => state.messages["getTelegramBotSettingsResponse"],
  (payload) => useSettingStore.getState().processGetTelegramBotSettings(payload),
);

useWebSocketStore.subscribe(
  (state) => state.messages["getNTPSettingsResponse"],
  (payload) => useSettingStore.getState().processGetNtpSettings(payload),
);

export default useSettingStore;
