import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useFunctionStore = create((set) => ({
  functions: [],
  temperatureCount: 0,
  relayCount: 0,

  processFunctions: (payload) => {
    if(payload.temperatureCount !== undefined) set({ temperatureCount: payload.temperatureCount });
    if(payload.relayCount !== undefined) set({ relayCount: payload.relayCount });

    if (!payload?.functions) return;

    set({ functions: payload.functions });
  },

  setFunctionIsEnabled: (functionIndex, isEnabled) => {
    const payload = {
      functionIndex,
      isEnabled,
    };

    useWebSocketStore.getState().sendMessage("setFunctionIsEnabled", payload);
  },

  setFunctionsSettings: (functions) => {
    const payload = {
      functionCount: functions.length,
      functions: functions,
    };

    useWebSocketStore.getState().sendMessage("setFunctionsSettings", payload);
  },
}));

useWebSocketStore.subscribe(
  (state) => state.messages["functionsData"],
  (payload) => useFunctionStore.getState().processFunctions(payload),
);

export default useFunctionStore;
