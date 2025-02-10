import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useTemperatureStore = create((set, get) => ({
  temperatures: [],
  temperatureSensorsCountCalculated: 0,
  temperatureSettings: [],
  discoveredTemperatureSensors: [],

  processTemperatures: (payload) => {
    let temperatureSensorsCountCalculated = 0;
    const formattedTemperatures = payload.temperatures.map(
      (temperature, index) => {
        temperatureSensorsCountCalculated++;
        return {
          id: index,
          value: temperature.toFixed(2),
        };
      }
    );

    set({ temperatures: formattedTemperatures, temperatureSensorsCountCalculated });
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

    useTemperatureStore.getState().getTemperatureSensorsSettings();
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
