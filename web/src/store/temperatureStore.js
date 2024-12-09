import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useTemperatureStore = create((set) => ({
  temperatures: [],

  processTemperatures: (data) => {
    if (!data?.temperatures) return;

    const fetchedTemperatures = data.temperatures;
    const half = Math.ceil(fetchedTemperatures.length / 2);
    
    let formattedTemperatures = [];

    for (let i = 0; i < half; i++) {
      formattedTemperatures.push({ id: i, value: fetchedTemperatures[i] });
      if (half + i < fetchedTemperatures.length) {
        formattedTemperatures.push({ id: half + i, value: fetchedTemperatures[half + i] });
      }
    }

    set({ temperatures: formattedTemperatures });
  },

  //   requestTemperatureUpdate: () => {
  //     useWebSocketStore.getState().sendMessage('temperature_request', {
  //       action: 'get_temperatures'
  //     });
  //   },

  //   deleteTemperature: (id) => {
  //     useWebSocketStore.getState().sendMessage('temperature_action', {
  //       action: 'delete',
  //       temperatureId: id
  //     });
  //   },

  //   updateTemperature: (id, newValue) => {
  //     useWebSocketStore.getState().sendMessage('temperature_action', {
  //       action: 'update',
  //       temperatureId: id,
  //       value: newValue
  //     });
  //   },
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

export default useTemperatureStore;
