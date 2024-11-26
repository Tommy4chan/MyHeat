import Bot from "../components/settings/Bot"
import Hardware from "../components/settings/Hardware"
import Wifi from "../components/settings/wifi"

const SettingsPage = () => {

  return (
    <div className="grid grid-cols-1 lg:grid-cols-3 items-start gap-6">
      <Wifi />
      <Bot />
      <Hardware />
    </div>
  )
}

export default SettingsPage