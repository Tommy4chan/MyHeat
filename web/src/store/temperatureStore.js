import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useTemperatureStore = create((set, get) => ({
  temperatures: [],
  temperatureSettings: [],
  discoveredTemperatureSensors: [],

  processTemperatures: (data) => {
    if (!data?.temperatures) return;

    const formattedTemperatures = data.temperatures.map(
      (temperature, index) => {
        return {
          id: index,
          value: temperature.toFixed(2),
        };
      }
    );

    set({ temperatures: formattedTemperatures });
  },

  getTemperatureSensorsSettings: () => {
    useWebSocketStore
      .getState()
      .sendMessage("getTemperatureSensorsSettings", {});
  },

  processTemperatureSettings: (data) => {
    if (!data?.payload) return;

    set({ temperatureSettings: data.payload });
  },

  setTemperatureSensorsSettings: (temperaturePin, temperatureCount) => {
    const payload = {
      temperaturePin,
      temperatureCount,
    };

    useWebSocketStore
      .getState()
      .sendMessage("setTemperatureSensorsSettings", payload);

    useTemperatureStore.getState().processTemperatureSettings({ payload });
  },

  getDiscoveredTemperatureSensors: () => {
    useWebSocketStore
      .getState()
      .sendMessage("getDiscoveredTemperatureSensors", {});
  },

  processDiscoveredTemperatureSensors: (data) => {
    if (!data?.payload) return;

    const discoveredTemperatureSensors =
      data.payload.discoveredTemperatureSensors.map((sensor, index) => {
        return {
          id: index,
          address: sensor,
          tempIndex: 0,
        };
      });

    set({ discoveredTemperatureSensors });
  },

  setTemperatureSensor: (tempIndex, sensorAddressIndex) => {
    const payload = {
      tempIndex,
      sensorAddressIndex,
    };

    const discoveredTemperatureSensors =
      get().discoveredTemperatureSensors.filter(
        (sensor) => sensor.id !== sensorAddressIndex
      );

    set({ discoveredTemperatureSensors });

    useWebSocketStore.getState().sendMessage("setTemperatureSensor", payload);
  },

  removeTemperatureSensor: (tempIndex) => {
    const payload = {
      tempIndex,
    };

    useWebSocketStore
      .getState()
      .sendMessage("deleteTemperatureSensor", payload);
  },
}));

useWebSocketStore.subscribe(
  (state) => state.messages["temperaturesData"],
  (message) => {
    if (message) {
      useTemperatureStore.getState().processTemperatures(message);
    }
  }
);

useWebSocketStore.subscribe(
  (state) => state.messages["getTemperatureSensorsSettingsResponse"],
  (message) => {
    if (message) {
      useTemperatureStore.getState().processTemperatureSettings(message);
    }
  }
);

useWebSocketStore.subscribe(
  (state) => state.messages["getDiscoveredTemperatureSensorsResponse"],
  (message) => {
    if (message) {
      useTemperatureStore
        .getState()
        .processDiscoveredTemperatureSensors(message);
    }
  }
);

export default useTemperatureStore;
