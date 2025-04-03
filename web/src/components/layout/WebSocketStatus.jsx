import React from "react";
import { ReadyState } from "react-use-websocket";
import useWebSocketStore from "@/store/websocketStore";
import Button from "@/components/ui/Button";

const WebSocketStatus = ({ className }) => {
  const { webSocketState, reconnect } = useWebSocketStore();

  const connectionStatus = {
    [ReadyState.CONNECTING]: {
      text: "Підключення",
      color: "text-yellow-500",
    },
    [ReadyState.OPEN]: {
      text: "Активне",
      color: "text-green-500",
    },
    [ReadyState.CLOSING]: {
      text: "Неактивне",
      color: "text-red-500",
    },
    [ReadyState.CLOSED]: {
      text: "Закрите",
      color: "text-red-500",
    },
    [ReadyState.UNINSTANTIATED]: {
      text: "Неініціалізоване",
      color: "text-purple-500",
    },
  }[webSocketState];

  return (
    <div className={`flex items-center gap-2 ${className}`}>
      <p>З'єданння: <span className={connectionStatus.color}>{connectionStatus.text}</span></p>
      {(webSocketState !== ReadyState.OPEN && webSocketState !== ReadyState.CONNECTING) && (
        <Button
          onClick={reconnect}
          buttonText="Під'єднатись"
          color="indigo"
          className="text-sm"
        />
      )}
    </div>
  );
};

export default WebSocketStatus;