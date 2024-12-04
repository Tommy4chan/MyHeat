import Bot from "../components/settings/Bot"
import Hardware from "../components/settings/Hardware"
import Device from "../components/settings/Device"
import NTP from "../components/settings/NTP"
import Wifi from "../components/settings/Wifi"
import Temperature from "../components/settings/Temperature"

const SettingsPage = () => {

  return (
    <div className="masonry">
      <Wifi />
      <Bot />
      <Hardware />
      <NTP />
      <Temperature />
      <Device />
    </div>
  )
}

export default SettingsPage