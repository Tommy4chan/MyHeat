import ColumnBlock from "../../../components/layout/ColumnBlock";
import { TemperatureSettingsForm } from "./TemperatureSettingsForm";
import { TemperatureSensorsList } from "./TemperatureSensorsList";

const TemperatureSettings = () => {
  return (
    <ColumnBlock>
      <h2 className="font-semibold text-2xl">Датчики температури</h2>
      <TemperatureSettingsForm />
      <TemperatureSensorsList />
    </ColumnBlock>
  )
}

export default TemperatureSettings