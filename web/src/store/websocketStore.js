import { create } from "zustand";
import { subscribeWithSelector } from "zustand/middleware";
import { ReadyState } from "react-use-websocket";

const useWebSocketStore = create(
  subscribeWithSelector((set, get) => ({
    messages: {},
    readyState: ReadyState.UNINSTANTIATED,
    isConnect: true,
    lastMessageTimestamp: null,
    INACTIVITY_TIMEOUT: 8000,

    onMessage: (event) => {
      try {
        const data = JSON.parse(event.data);
        const { messageType, ...payload } = data;

        if (messageType) {
          set({
            messages: {
              ...get().messages,
              [messageType]: payload,
            },
            lastMessageTimestamp: Date.now(),
          });
        }
      } catch (error) {
        console.error("WebSocket message parsing error:", error);
      }
    },

    setSendMessage: (sendMessage) => {
      set({ sendMessage });
    },

    sendMessage: () => {},

    reconnect: () => {
      console.log("Manually reconnecting WebSocket...");

      set({ isConnect: true });
    },

    setReadyState: (readyState) => {
      set({ readyState });
    },

    setIsConnect: (isConnect) => {
      set({ isConnect });
    },
  }))
);

export default useWebSocketStore;
