import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useRelayStore = create((set) => ({
  relays: [],
  relayCount: 1,
  relaysSettings: [],

  processRelays: (payload) => {
    set({ relays: payload.relays });
  },

  getRelayCount: () => {
    useWebSocketStore.getState().sendMessage("getRelayCount", {});
  },

  processRelaysCount: (payload) => {
    set({ relayCount: payload.relayCount });
  },

  setRelayCount: (relayCount) => {
    const payload = {
      relayCount,
    };

    useWebSocketStore.getState().sendMessage("setRelayCount", payload);

    useRelayStore.getState().processRelaysCount({ payload });
    useRelayStore.getState().getRelaysSettings();
  },

  getRelaysSettings: () => {
    useWebSocketStore.getState().sendMessage("getRelaysSettings", {});
  },

  processRelaysSettings: (payload) => {
    const relays = payload.relays.map((relay) => {
      return {
        pin: parseInt(relay.pin),
        isActiveOnHigh: +relay.isActiveOnHigh,
      };
    });

    set({ relaysSettings: relays });
  },

  setRelayMode: (relayIndex, relayMode) => {
    const payload = {
      relayIndex,
      relayMode,
    };

    useWebSocketStore.getState().sendMessage("setRelayMode", payload);
  },

  setRelaysSettings: (relays) => {
    relays = relays.map((relay) => {
      return {
        pin: parseInt(relay.pin),
        isActiveOnHigh: parseInt(relay.isActiveOnHigh),
      };
    });

    const payload = {
      relays,
    };

    useWebSocketStore.getState().sendMessage("setRelaysSettings", payload);

    useRelayStore.getState().processRelaysSettings({ payload });
  }
}));

useWebSocketStore.subscribe(
  (state) => state.messages["relaysData"],
  (payload) => useRelayStore.getState().processRelays(payload),
);

useWebSocketStore.subscribe(
  (state) => state.messages["getRelayCountResponse"],
  (payload) => useRelayStore.getState().processRelaysCount(payload),
);

useWebSocketStore.subscribe(
  (state) => state.messages["getRelaysSettingsResponse"],
  (payload) => useRelayStore.getState().processRelaysSettings(payload),
);

export default useRelayStore;
