import Bot from "./components/Bot"
import Hardware from "./components/Hardware"
import Device from "./components/Device"
import NTP from "./components/NTP"
import Wifi from "./components/Wifi"

const SettingsPage = () => {

  return (
    <div className="masonry">
      <Wifi />
      <Bot />
      <Hardware />
      <NTP />
      <Device />
    </div>
  )
}

export default SettingsPage