import { create } from "zustand";
import useWebSocketStore from "./websocketStore";
import { ReadyState } from "react-use-websocket";

const useSettingStore = create((set, get) => ({
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
  smokeSensorSettings: {},
  smokeSensor: {},
  mqttSettings: {},

  setWifiSettings: (wifiSSID, wifiPassword, apSSID, apPassword, isFallbackAPEnabled, mDNS, deviceName) => {
    isFallbackAPEnabled = convertToBoolean(isFallbackAPEnabled);

    const payload = {
      wifiSSID,
      wifiPassword,
      apSSID,
      apPassword,
      isFallbackAPEnabled,
      mDNS,
      deviceName,
    };
    
    useWebSocketStore.getState().sendMessage("setWifiSettings", payload);

    if (mDNS !== get().wifiSettings["mDNS"] && isOpenedByMdns()) {
      setTimeout(() => {
        location.replace(`http://${mDNS}.local`);
      }, 1500);
    }
    else {
      get().getWifiSettings();
    }
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

  setTelegramBotSettings: (token, registerPhrase, isEnabled, isAlertNotificationsEnabled, isBootNotificationEnabled) => {
    isEnabled = convertToBoolean(isEnabled);
    isAlertNotificationsEnabled = convertToBoolean(isAlertNotificationsEnabled);
    isBootNotificationEnabled = convertToBoolean(isBootNotificationEnabled);

    const payload = {
      token,
      registerPhrase,
      isEnabled,
      isAlertNotificationsEnabled,
      isBootNotificationEnabled,
    };

    useWebSocketStore.getState().sendMessage("setTelegramBotSettings", payload);
    get().getTelegramBotSettings();
  },

  getTelegramBotSettings: () => {
    useWebSocketStore.getState().sendMessage("getTelegramBotSettings");
  },

  processGetTelegramBotSettings: (payload) => {
    set({ telegramBotSettings: payload });
  },

  deleteTelegramUser: (chatId) => {
    useWebSocketStore.getState().sendMessage("deleteTelegramUser", { chatId });
    get().getTelegramBotSettings();
  },

  setNtpSettings: (ntpServer, ntpIANA, ntpTZ) => {
    const payload = {
      ntpServer,
      ntpIANA,
      ntpTZ,
    };

    useWebSocketStore.getState().sendMessage("setNTPSettings", payload);
    get().getNtpSettings();
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
    isEnabled,
    oledType,
    oledAddress,
    screenPowerSaveInterval,
    isReturnHomeOnSleep,
    oledPins,
    encInvert,
    encoderPins
  ) => {
    isEnabled = convertToBoolean(isEnabled);
    isReturnHomeOnSleep = convertToBoolean(isReturnHomeOnSleep);
    encInvert = convertToBoolean(encInvert);

    const payload = {
      isEnabled,
      oledType: parseInt(oledType),
      oledAddress: parseInt(oledAddress, 16),
      screenPowerSaveInterval,
      isReturnHomeOnSleep,
      oledSDA: oledPins[0],
      oledSCL: oledPins[1],
      encA: encoderPins[0],
      encB: encoderPins[1],
      encBtn: encoderPins[2],
      encInvert,
    };

    useWebSocketStore.getState().sendMessage("setHardwareIOSettings", payload);
    get().getHardwareIOSettings();
  },

  getAllDeviceSettings: () => {
    useWebSocketStore.getState().sendMessage("getAllDeviceSettings");
  },

  processGetAllDeviceSettings: (payload) => {
    set({ allDeviceSettings: payload });
  },

  setAllDeviceSettings: (payload) => {
    useWebSocketStore.getState().sendMessage("setAllDeviceSettings", payload);

    // Over an IP or a tunnel address the page stays where it is, only the .local name changes with the settings
    if (!isOpenedByMdns()) return;

    setTimeout(() => {
      location.replace(`http://${payload["MyHeatWifi"]["mDNS"]}.local`);
    }, 1500);
  },

  restartDevice: () => {
    useWebSocketStore.getState().sendMessage("restartDevice");
  },

  getSmokeSensorSettings: () => {
    useWebSocketStore.getState().sendMessage("getSmokeSensorSettings");
  },

  processGetSmokeSensorSettings: (payload) => {
    set({ smokeSensorSettings: payload });
  },

  setSmokeSensorSettings: (isEnabled, threshold, pin) => {
    isEnabled = convertToBoolean(isEnabled);

    const payload = {
      isEnabled,
      threshold,
      pin,
    };

    useWebSocketStore.getState().sendMessage("setSmokeSensorSettings", payload);
    get().getSmokeSensorSettings();
  },

  getSmokeSensor: () => {
    useWebSocketStore.getState().sendMessage("getSmokeSensor");
  },

  processGetSmokeSensor: (payload) => {
    set({ smokeSensor: payload });
  },

  setMqttSettings: (broker, port, user, password, isEnabled, publishInterval, isBootNotificationEnabled) => {
    isEnabled = convertToBoolean(isEnabled);
    isBootNotificationEnabled = convertToBoolean(isBootNotificationEnabled);

    const payload = {
      broker,
      port,
      user,
      password,
      isEnabled,
      publishInterval: Number(publishInterval),
      isBootNotificationEnabled,
    };

    useWebSocketStore.getState().sendMessage("setMqttSettings", payload);
    get().getMqttSettings();
  },

  getMqttSettings: () => {
    useWebSocketStore.getState().sendMessage("getMqttSettings");
  },

  processGetMqttSettings: (payload) => {
    set({ mqttSettings: payload });
  },

}));

// The redirect to the new mDNS name only makes sense when the page was opened by that name
const isOpenedByMdns = () => location.hostname.endsWith(".local");

const convertToBoolean = (value) => value === 'true' || value === 'false' ? value === 'true' : value;

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

useWebSocketStore.subscribe(
  (state) => state.messages["getSmokeSensorSettingsResponse"],
  (payload) => useSettingStore.getState().processGetSmokeSensorSettings(payload)
);

useWebSocketStore.subscribe(
  (state) => state.messages["getSmokeSensorResponse"],
  (payload) => useSettingStore.getState().processGetSmokeSensor(payload)
);

useWebSocketStore.subscribe(
  (state) => state.messages["getMqttSettingsResponse"],
  (payload) => useSettingStore.getState().processGetMqttSettings(payload)
);

useWebSocketStore.subscribe(
  (state) => state.webSocketState,
  (webSocketState) => {
    if (webSocketState !== ReadyState.OPEN) {
      useSettingStore.setState({ isScanningForWifiNetworks: false });
    }
  }
);

export default useSettingStore;
