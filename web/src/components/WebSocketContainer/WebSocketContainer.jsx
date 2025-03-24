import useWebSocket, { ReadyState } from "react-use-websocket";
import useWebSocketStore from "@/store/websocketStore";
import { useEffect } from "react";

const WebSocketProvider = ({ children }) => {

  const { onMessage, setSendMessage, setWebSocketState, setIsConnect, INACTIVITY_TIMEOUT, isConnect } = useWebSocketStore();

  const { sendJsonMessage, readyState: webSocketState } = useWebSocket(
    import.meta.env.DEV ? "ws://192.168.137.96/ws" : `ws://${location.host}/ws`,
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
            }, false);
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
    setWebSocketState(webSocketState);

    const interval = setInterval(() => {
      const now = Date.now();
      const { lastMessageTimestamp } = useWebSocketStore.getState();

      if (
        webSocketState === ReadyState.OPEN &&
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
  }, [webSocketState]);

  return children;
};

export default WebSocketProvider;