import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useFunctionStore = create((set) => ({
  functions: [],

  processFunctions: (data) => {
    if (!data?.functions) return;

    const updatedFunctions = data.functions.map((func) => ({
      ...func,
      deltaValueSign: func.deltaValue.map((val) => (val < 0 ? 0 : 1)),
      deltaValue: func.deltaValue.map((val) => Math.abs(val)),
    }));

    set({ functions: updatedFunctions });
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
