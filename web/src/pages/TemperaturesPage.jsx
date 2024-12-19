import { useEffect, useState, useMemo, useCallback } from "react";
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
    getTemperatureSensorsSettings,
    setTemperatureSensorsSettings,
    getDiscoveredTemperatureSensors,
    discoveredTemperatureSensors,
    setTemperatureSensor,
  } = useTemperatureStore();

  const [settings, setSettings] = useState({
    temperaturePin: 0,
    temperatureCount: 1,
  });

  const [temperatureSensors, setTemperatureSensors] = useState([]);

  const temperatureOptions = useMemo(
    () =>
      Array.from({ length: temperatureSettings.temperatureCount }, (_, i) => ({
        value: i,
        text: `T${i}`,
      })),
    [temperatureSettings.temperatureCount]
  );

  useEffect(() => {
    getTemperatureSensorsSettings();
  }, []);

  useEffect(() => {
    setTemperatureSensors(discoveredTemperatureSensors);
  }, [discoveredTemperatureSensors]);

  useEffect(() => {
    setSettings({
      temperaturePin: temperatureSettings.temperaturePin,
      temperatureCount: temperatureSettings.temperatureCount,
    });
  }, [temperatureSettings]);

  const handleSettingsChange = (key) => (e) => {
    if (key === "temperaturePin") {
      handlePinChange(e, handleTemperaturePinChange, settings.temperaturePin);
    }
    else {
      setSettings((prev) => ({ ...prev, [key]: e.target.value }));
    }
  };

  const handleTemperaturePinChange = useCallback((newPin) => {
    setSettings({ ...settings, temperaturePin: newPin });
  }, [settings]);

  const handleSettingsSave = useCallback(() => {
    setTemperatureSensorsSettings(
      settings.temperaturePin,
      settings.temperatureCount
    );
  }, [settings]);

  const handleSensorTempIndexChange = useCallback(
    (sensorAddressIndex) => (e) => {
      setTemperatureSensors((prev) =>
        prev.map((sensor) =>
          sensor.id === sensorAddressIndex
            ? { ...sensor, tempIndex: e.target.value }
            : sensor
        )
      );
    },
    []
  );

  const handleSensorSave = useCallback(
    (index) => () => {
      const sensor = temperatureSensors[index];
      setTemperatureSensors((prev) =>
        prev.filter((_, i) => i !== index)
      );
      setTemperatureSensor(sensor.tempIndex, sensor.id);
    },
    [temperatureSensors, setTemperatureSensor]
  );

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
                value={settings.temperaturePin}
                options={getAvailableInputPins(settings.temperaturePin)}
                onChange={handleSettingsChange("temperaturePin")}
                className='w-full'
              />
            </FormField>
            <FormField label='Кількість датчиків'>
              <Input
                value={settings.temperatureCount}
                onChange={handleSettingsChange("temperatureCount")}
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
          {temperatureSensors?.map((temperatureSensor, index) => (
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
                  onChange={handleSensorTempIndexChange(temperatureSensor.id)}
                />
                <SaveButton onClick={handleSensorSave(index)} />
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