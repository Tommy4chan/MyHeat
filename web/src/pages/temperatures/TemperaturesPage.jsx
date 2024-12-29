import Temperatures from "@/components/Temperatures";
import TemperatureSettings from "./components/TemperatureSettings";

const TemperaturesPage = () => {
  return (
    <div className="grid grid-cols-1 lg:grid-cols-2 items-start gap-6">
      <Temperatures isDeleteVisible={true} />
      <TemperatureSettings />
    </div>
  )
}

export default TemperaturesPage