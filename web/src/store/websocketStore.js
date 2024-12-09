import { create } from 'zustand';
import { subscribeWithSelector } from 'zustand/middleware'

const useWebSocketStore = create(subscribeWithSelector((set, get) => ({
  socket: null,
  isConnected: false,
  messages: {},

  connect: () => {
    if (get().socket) return;

    const ws = new WebSocket("ws://192.168.1.228/ws");

    ws.onopen = () => {
      set({ isConnected: true, socket: ws });
      console.log("WebSocket connection established");
    };

    ws.onmessage = (event) => {
      try {
        const data = JSON.parse(event.data);
        const { messageType, ...payload } = data;

        if (messageType) {
          set((state) => ({
            messages: {
              ...state.messages,
              [messageType]: payload,
            },
          }));
        }
      } catch (error) {
        console.error("Error parsing WebSocket message:", error);
      }
    };

    ws.onerror = (error) => {
      console.error("WebSocket Error:", error);
    };

    ws.onclose = () => {
      set({ isConnected: false, socket: null });
    };

    set({ socket: ws });
  },

  sendMessage: (messageType, payload) => {
    const { socket } = get();
    if (socket && socket.readyState === WebSocket.OPEN) {
      socket.send(
        JSON.stringify({
          messageType,
          ...payload,
        })
      );
    } else {
      console.error("WebSocket is not connected");
    }
  },

  disconnect: () => {
    const { socket } = get();
    if (socket) {
      socket.close();
      set({ socket: null, isConnected: false, messages: {} });
    }
  },
})));

export default useWebSocketStore;
