import { create } from "zustand";
import { subscribeWithSelector } from "zustand/middleware";
import useWebSocket from "react-use-websocket";

const useWebSocketStore = create(
  subscribeWithSelector((set) => ({
    messages: {},
    isConnected: false,
    sendMessage: () => {},
  }))
);

export const WebSocketProvider = ({ children }) => {
  const { sendJsonMessage, lastJsonMessage, readyState } = useWebSocket(
    "ws://192.168.1.228/ws",
    {
      retryOnError: true,
      shouldReconnect: () => true,
      reconnectInterval: 1000,
      reconnectAttempts: 5,
      share: true,
      heartbeat: true,
      onMessage: (event) => {
        try {
          const data = JSON.parse(event.data);
          const { messageType, ...payload } = data;

          if (messageType && messageType !== "pong") {
            useWebSocketStore.setState((state) => ({
              messages: {
                ...state.messages,
                [messageType]: payload,
              },
            }));
          }
        } catch (error) {
          console.error("WebSocket message parsing error:", error);
        }
      },
      onOpen: () => {
        useWebSocketStore.setState({
          isConnected: true,
          sendMessage: (messageType, payload) => {
            sendJsonMessage({
              messageType,
              payload,
            });
          },
        });
      },
      onClose: () => {
        console.log("WebSocket disconnected");
        useWebSocketStore.setState({ isConnected: false });
      },
    }
  );

  return children;
};

export default useWebSocketStore;
