import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useRelayStore = create((set) => ({
  relays: [],

  processRelays: (data) => {
    if (!data?.relays) return;

    set({ relays: data.relays });
  }
}));

useWebSocketStore.subscribe(
  (state) => state.messages["relaysData"],
  (message) => {
    if (message) {
      useRelayStore.getState().processRelays(message);
    }
  }
);

export default useRelayStore;
