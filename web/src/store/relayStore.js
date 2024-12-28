import { create } from "zustand";
import useWebSocketStore from "./websocketStore";

const useRelayStore = create((set) => ({
  relays: [],
  relayCount: 1,
  relaysSettings: [],

  processRelays: (data) => {
    if (!data?.relays) return;

    set({ relays: data.relays });
  },

  getRelayCount: () => {
    useWebSocketStore.getState().sendMessage("getRelayCount", {});
  },

  processRelaysCount: (data) => {
    if (!data?.payload) return;

    set({ relayCount: data.payload.relayCount });
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

  processRelaysSettings: (data) => {
    if (!data?.payload) return;

    const relays = data.payload.relays.map((relay) => {
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
  (message) => {
    if (message) {
      useRelayStore.getState().processRelays(message);
    }
  }
);

useWebSocketStore.subscribe(
  (state) => state.messages["getRelayCountResponse"],
  (message) => {
    if (message) {
      useRelayStore.getState().processRelaysCount(message);
    }
  }
);

useWebSocketStore.subscribe(
  (state) => state.messages["getRelaysSettingsResponse"],
  (message) => {
    if (message) {
      useRelayStore.getState().processRelaysSettings(message);
    }
  }
);

export default useRelayStore;
