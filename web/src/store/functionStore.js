import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useFunctionStore = create((set) => ({
  functions: [],
  temperatureCount: 0,
  relayCount: 0,
  tnIndex: 255,
  tUnknownIndex: 254,
  relayUnknownIndex: 255,

  processFunctions: (data) => {
    if(!data.temperatureCount || !data.relayCount) return;

    set({ temperatureCount: data.temperatureCount });
    set({ relayCount: data.relayCount });

    if (!data?.functions) return;

    const updatedFunctions = data.functions.map((func) => ({
      ...func,
      deltaValueSign: func.deltaValue.map((val) => (val < 0 ? 0 : 1)),
      deltaValue: func.deltaValue.map((val) => Math.abs(val)),
    }));

    set({ functions: updatedFunctions });
  },

  setFunctionIsEnabled: (functionIndex, isEnabled) => {
    const payload = {
      functionIndex,
      isEnabled,
    };

    useWebSocketStore.getState().sendMessage("setFunctionIsEnabled", payload);
  },

  setFunctionsSettings: (functions) => {
    const formattedFunctions = functions.map((func) => ({
      ...func,
      deltaValue: func.deltaValue.map((val, index) => (func.deltaValueSign[index] ? val : -val)),
    }));

    const payload = {
      functionCount: formattedFunctions.length,
      functions: formattedFunctions,
    };

    useWebSocketStore.getState().sendMessage("setFunctionsSettings", payload);
  },
}));

useWebSocketStore.subscribe(
  (state) => state.messages["functionsData"],
  (message) => {
    if (message) {
      useFunctionStore.getState().processFunctions(message);
    }
  }
);

export default useFunctionStore;
