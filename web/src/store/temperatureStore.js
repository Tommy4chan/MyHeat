import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useTemperatureStore = create((set, get) => ({
  temperatures: [],
  temperatureSettings: [],
  discoveredTemperatureSensors: [],

  processTemperatures: (payload) => {
    const formattedTemperatures = payload.temperatures.map(
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

  processTemperatureSettings: (payload) => {
    set({ temperatureSettings: payload });
  },

  setTemperatureSensorsSettings: (temperaturePin, temperatureCount) => {
    const payload = {
      temperaturePin,
      temperatureCount,
    };

    useWebSocketStore
      .getState()
      .sendMessage("setTemperatureSensorsSettings", payload);

    useTemperatureStore.getState().processTemperatureSettings(payload);
  },

  getDiscoveredTemperatureSensors: () => {
    useWebSocketStore
      .getState()
      .sendMessage("getDiscoveredTemperatureSensors", {});
  },

  processDiscoveredTemperatureSensors: (payload) => {
    const discoveredTemperatureSensors =
      payload.discoveredTemperatureSensors.map((sensor, index) => {
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
  (payload) => useTemperatureStore.getState().processTemperatures(payload)
);

useWebSocketStore.subscribe(
  (state) => state.messages["getTemperatureSensorsSettingsResponse"],
  (payload) => useTemperatureStore.getState().processTemperatureSettings(payload)
);

useWebSocketStore.subscribe(
  (state) => state.messages["getDiscoveredTemperatureSensorsResponse"],
  (payload) => useTemperatureStore.getState().processDiscoveredTemperatureSensors(payload)
);

export default useTemperatureStore;
