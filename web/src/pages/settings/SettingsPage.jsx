import Bot from "./components/Bot"
import Hardware from "./components/Hardware"
import Device from "./components/Device"
import NTP from "./components/NTP"
import Wifi from "./components/Wifi"

const SettingsPage = () => {

  return (
    <div className="grid grid-cols-1 lg:grid-cols-3 gap-4">
      <div className="flex flex-col gap-4">
        <Wifi />
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