import Bot from "./components/Bot"
import Hardware from "./components/Hardware"
import Device from "./components/Device"
import NTP from "./components/NTP"
import Wifi from "./components/Wifi"
import Mqtt from "./components/Mqtt"
import SmokeSensor from "./components/SmokeSensor"

import { useEffect } from "react"
import useWebSocketStore from "@/store/websocketStore"
import useSettingStore from "@/store/settingStore"
import { ReadyState } from "react-use-websocket"

const SettingsPage = () => {
  const webSocketState = useWebSocketStore(state => state.webSocketState);
  const { getWifiSettings, getMqttSettings, getSmokeSensorSettings, getTelegramBotSettings, getHardwareIOSettings, getNtpSettings } = useSettingStore();

  useEffect(() => {
    if (webSocketState === ReadyState.OPEN) {
      getWifiSettings();
      getMqttSettings();
      getSmokeSensorSettings();
      getTelegramBotSettings();
      getHardwareIOSettings();
      getNtpSettings();
    }
  }, [webSocketState]);

  return (
    <div className="grid grid-cols-1 lg:grid-cols-3 gap-4">
      <div className="flex flex-col gap-4">
        <Wifi />
        <Mqtt />
        <SmokeSensor />
      </div>
      <div className="flex flex-col gap-4">
        <Bot />
        <Hardware />
      </div>
      <div className="flex flex-col gap-4">
        <NTP />
        <Device />
      </div>
    </div>
  )
}

export default SettingsPage