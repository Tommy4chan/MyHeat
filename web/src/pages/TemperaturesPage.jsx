import { useEffect, useState } from "react";
import Temperatures from "../components/Temperatures"
import DarkWrapperBlock from "../components/ui/DarkWrapperBlock";
import Button from "../components/ui/buttons/Button";
import Select from "../components/ui/Select";
import ColumnBlock from "../components/ui/ColumnBlock";
import SaveButton from "../components/ui/buttons/SaveButton";
import WrapperBlock from "../components/ui/WrapperBlock";
import usePinStore from "../store/pinStore";
import { handlePinChange } from "../utils/pinHandler";
import FormField from "../components/ui/FormField";
import Input from "../components/ui/Input";
import useTemperatureStore from "../store/temperatureStore";

const TemperaturesPage = () => {
  const {
    getAvailableInputPins,
  } = usePinStore();

  const {
    temperatureSettings,
    getTemperatureSensorSettings,
    setTemperatureSensorSettings,
    getDiscoveredTemperatureSensors,
    discoveredTemperatureSensors,
    setTemperatureSensor,
  } = useTemperatureStore();

  const [temperaturePin, setTemperaturePin] = useState(0);
  const [temperatureSensorsCount, setTemperatureSensorsCount] = useState(1);
  const [temperatureOptions, setTemperatureOptions] = useState([]);
  const [temperatureSensorsAddresses, setTemperatureSensorsAddresses] = useState([]);

  useEffect(() => {
    getTemperatureSensorSettings();
  }, []);

  useEffect(() => {
    setTemperatureSensorsAddresses(discoveredTemperatureSensors);
  }, [discoveredTemperatureSensors]);

  useEffect(() => {
    setTemperaturePin(temperatureSettings['temperaturePin']);
    setTemperatureSensorsCount(temperatureSettings['temperatureCount']);
    setTemperatureOptions(Array.from({ length: temperatureSensorsCount }, (_, i) => ({ value: i, text: `T${i}` })));
  }, [temperatureSettings]);

  const handleTemperaturePinChange = (e) => {
    handlePinChange(e, setTemperaturePin, temperaturePin);
  }

  const handleSettingsSave = () => {
    setTemperatureSensorSettings(
      temperaturePin,
      temperatureSensorsCount,
    );

    setTemperatureOptions(Array.from({ length: temperatureSensorsCount }, (_, i) => ({ value: i, text: `T${i}` })));
  }

  const handleSensorTempIndexChange = (e, sensorAddressIndex) => {
    setTemperatureSensorsAddresses(temperatureSensorsAddresses.map((sensor) => {
      if (sensor.id === sensorAddressIndex) {
        return {
          ...sensor,
          tempIndex: e.target.value,
        }
      }
    }));
  };

  const handleSensorSave = (index) => {
    const sensorAddressIndex = temperatureSensorsAddresses[index].id;
    const tempIndex = temperatureSensorsAddresses[index].tempIndex;
    
    setTemperatureSensorsAddresses(temperatureSensorsAddresses.filter((sensor) => sensor.id !== sensorAddressIndex));

    setTemperatureSensor(tempIndex, sensorAddressIndex);
  }

  return (
    <div className="grid grid-cols-1 lg:grid-cols-2 items-start gap-6">
      <Temperatures isDeleteVisible={true} />
      <ColumnBlock>
        <h2 className="font-semibold text-2xl">Датчики температури</h2>
        <WrapperBlock>
          <h3 className='text-xl'>Налаштування:</h3>
          <DarkWrapperBlock className="md:!flex-col">
            <FormField label='Пін'>
              <Select
                value={temperaturePin}
                options={getAvailableInputPins(temperaturePin)}
                onChange={handleTemperaturePinChange}
                className='w-full'
              />
            </FormField>
            <FormField label='Кількість датчиків'>
              <Input
                value={temperatureSensorsCount}
                onChange={(e) => setTemperatureSensorsCount(e.target.value)}
                isNumber={true}
                maxLength={3}
                className='w-full'
              />
            </FormField>
          </DarkWrapperBlock>
          <SaveButton onClick={handleSettingsSave} />
        </WrapperBlock>
        <WrapperBlock>
          <h3 className='text-xl'>Датчики:</h3>
          {temperatureSensorsAddresses?.map((temperatureSensor, index) => (
            <DarkWrapperBlock className="justify-between" key={temperatureSensor.id}>
              <p
                className="text-lg text-gray-300"
              >
                Датчик {index}: {temperatureSensor.address}
              </p>
              <div className="flex gap-2 flex-col md:flex-row">
                <Select
                  value={temperatureSensor.tempIndex}
                  options={temperatureOptions}
                  onChange={(e) => handleSensorTempIndexChange(e, temperatureSensor.id)}
                />
                <SaveButton onClick={() => handleSensorSave(index)} />
              </div>
            </DarkWrapperBlock>
          ))}
          <Button buttonText={'Виявити датчики'} color="indigo" onClick={getDiscoveredTemperatureSensors} />
        </WrapperBlock>
      </ColumnBlock>
    </div>
  )
}

export default TemperaturesPage