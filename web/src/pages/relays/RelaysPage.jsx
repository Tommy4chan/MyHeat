import { Relays } from "../../components/Relays"
import { RelaysSettings } from "./components/RelaysSettings"

const RelaysPage = () => {

  return (
    <div className="grid grid-cols-1 lg:grid-cols-2 items-start gap-6">
      <Relays />
      <RelaysSettings />
    </div>
  )
}

export default RelaysPage