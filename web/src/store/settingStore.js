import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useSettingStore = create((set) => ({
  wifiSettings: {},
  scannedWifiNetworks: [],
  isScanningForWifiNetworks: false,
  telegramBotSettings: {},
  ntpSettings: {},
  hardwareIOSettings: {
    oledPins: [0, 0],
    encoderPins: [0, 0, 0],
  },
  allDeviceSettings: {},

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
    isActive = isActive === 'true' || isActive === 'false' ? isActive === 'true' : isActive;

    const payload = {
      token,
      registerPhrase,
      isActive,
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

  getHardwareIOSettings: () => {
    useWebSocketStore.getState().sendMessage("getHardwareIOSettings");
  },

  processGetHardwareIOSettings: (payload) => {
    payload["screenPowerSaveInterval"] = payload["screenPowerSaveInterval"];
    payload["oledPins"] = [payload["oledSDA"], payload["oledSCL"]];
    payload["encoderPins"] = [
      payload["encA"],
      payload["encB"],
      payload["encBtn"],
    ];
    payload["oledAddress"] = "0x" + payload["oledAddress"].toString(16);

    set({ hardwareIOSettings: payload });
  },

  setHardwareIOSettings: (
    isActive,
    oledAddress,
    screenPowerSaveInterval,
    oledPins,
    encInvert,
    encoderPins
  ) => {
    isActive = isActive === 'true' || isActive === 'false' ? isActive === 'true' : isActive;
    encInvert = encInvert === 'true' || encInvert === 'false' ? encInvert === 'true' : encInvert;

    const payload = {
      isActive,
      oledAddress: parseInt(oledAddress, 16),
      screenPowerSaveInterval,
      oledSDA: oledPins[0],
      oledSCL: oledPins[1],
      encA: encoderPins[0],
      encB: encoderPins[1],
      encBtn: encoderPins[2],
      encInvert,
    };

    useWebSocketStore.getState().sendMessage("setHardwareIOSettings", payload);
  },

  getAllDeviceSettings: () => {
    useWebSocketStore.getState().sendMessage("getAllDeviceSettings");
  },

  processGetAllDeviceSettings: (payload) => {
    set({ allDeviceSettings: payload });
  }

}));

useWebSocketStore.subscribe(
  (state) => state.messages["getWifiSettingsResponse"],
  (payload) => useSettingStore.getState().processGetWifiSettings(payload)
);

useWebSocketStore.subscribe(
  (state) => state.messages["wifiScanData"],
  (payload) => useSettingStore.getState().processWifiScanData(payload)
);

useWebSocketStore.subscribe(
  (state) => state.messages["getTelegramBotSettingsResponse"],
  (payload) => useSettingStore.getState().processGetTelegramBotSettings(payload)
);

useWebSocketStore.subscribe(
  (state) => state.messages["getNTPSettingsResponse"],
  (payload) => useSettingStore.getState().processGetNtpSettings(payload)
);

useWebSocketStore.subscribe(
  (state) => state.messages["getHardwareIOSettingsResponse"],
  (payload) => useSettingStore.getState().processGetHardwareIOSettings(payload)
);

useWebSocketStore.subscribe(
  (state) => state.messages["getAllDeviceSettingsResponse"],
  (payload) => useSettingStore.getState().processGetAllDeviceSettings(payload)
);

export default useSettingStore;
