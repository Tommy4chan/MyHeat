import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useTemperatureStore = create((set) => ({
  temperatures: [],
  temperatureSettings: [],
  discoveredTemperatureSensors: [],

  processTemperatures: (data) => {
    if (!data?.temperatures) return;

    const fetchedTemperatures = data.temperatures;
    const half = Math.ceil(fetchedTemperatures.length / 2);

    let formattedTemperatures = [];

    for (let i = 0; i < half; i++) {
      formattedTemperatures.push({ id: i, value: fetchedTemperatures[i] });
      if (half + i < fetchedTemperatures.length) {
        formattedTemperatures.push({
          id: half + i,
          value: fetchedTemperatures[half + i],
        });
      }
    }

    set({ temperatures: formattedTemperatures });
  },

  getTemperatureSensorSettings: () => {
    useWebSocketStore
      .getState()
      .sendMessage("getTemperatureSensorSettings", {});
  },

  processTemperatureSettings: (data) => {
    if (!data?.payload) return;

    set({ temperatureSettings: data.payload });
  },

  setTemperatureSensorSettings: (temperaturePin, temperatureCount) => {
    const payload = {
      temperaturePin,
      temperatureCount,
    };

    useWebSocketStore
      .getState()
      .sendMessage("setTemperatureSensorSettings", payload);
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

// Subscribe only to temperature messages
useWebSocketStore.subscribe(
  (state) => state.messages["temperaturesData"],
  (message) => {
    if (message) {
      useTemperatureStore.getState().processTemperatures(message);
    }
  }
);

useWebSocketStore.subscribe(
  (state) => state.messages["getTemperatureSensorSettingsResponse"],
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
