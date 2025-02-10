import useWebSocket, { ReadyState } from "react-use-websocket";
import useWebSocketStore from "@/store/websocketStore";
import { useEffect } from "react";

const WebSocketProvider = ({ children }) => {

  const { onMessage, setSendMessage, setReadyState, setIsConnect, INACTIVITY_TIMEOUT, isConnect } = useWebSocketStore();

  const { sendJsonMessage, readyState } = useWebSocket(
    "ws://192.168.1.228/ws",
    // `ws://${location.host}/ws`,
    {
      retryOnError: true,
      shouldReconnect: () => true,
      reconnectInterval: 1000,
      reconnectAttempts: 3,
      share: true,
      onMessage: onMessage,
      onOpen: () => {
        console.log("WebSocket connected");
        setSendMessage(
          (messageType, payload) => {
            sendJsonMessage({
              messageType,
              payload,
            });
          }
        );
      },
      onClose: () => {
        console.log("WebSocket disconnected");
      },
      onReconnectStop: () => {
        console.log("WebSocket reconnecting stopped");

        setIsConnect(false);
      },
    },
    isConnect
  );

  useEffect(() => {
    setReadyState(readyState);

    const interval = setInterval(() => {
      const now = Date.now();
      const { lastMessageTimestamp } = useWebSocketStore.getState();

      if (
        readyState === ReadyState.OPEN &&
        lastMessageTimestamp &&
        now - lastMessageTimestamp > INACTIVITY_TIMEOUT
      ) {
        console.warn(
          "WebSocket connection marked as inactive due to inactivity"
        );
        setIsConnect(false);
      }
    }, 1000);

    return () => clearInterval(interval);
  }, [readyState]);

  return children;
};

export default WebSocketProvider;