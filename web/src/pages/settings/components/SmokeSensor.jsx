import Input from "../../../components/ui/Input";
import DarkWrapperBlock from "../../../components/layout/DarkWrapperBlock";
import SaveButton from "../../../components/ui/SaveButton";
import SettingsForm from "../../../components/layout/SettingsForm";
import FormField from "../../../components/ui/FormField";
import SelectToggle from "../../../components/ui/SelectToggle";
import WrapperBlock from "../../../components/layout/WrapperBlock";
import { useEffect, useState } from "react";
import usePinStore from "../../../store/pinStore";
import Select from "../../../components/ui/Select";
import useSettingStore from "../../../store/settingStore";

const SmokeSensor = () => {
  const {
    handlePinChange,
    getAvailableInputPins,
  } = usePinStore();

  const { smokeSensorSettings, smokeSensor, getSmokeSensor, setSmokeSensorSettings, getSmokeSensorSettings } = useSettingStore();

  const [sensorValue, setSensorValue] = useState(0);
  const [isSensorOverThreshold, setIsSensorOverThreshold] = useState(false);
  const [isSensorEnabled, setIsSensorEnabled] = useState(false);
  const [sensorTreshold, setSensorTreshold] = useState(2500);
  const [sensorPin, setSensorPin] = useState(0);

  useEffect(() => {
    getSmokeSensorSettings();
    getSmokeSensor();
    
    const interval = setInterval(() => {
      getSmokeSensor();
      console.log('getSmokeSensor');
    }, 2000);

    return () => clearInterval(interval);
  }, []);

  useEffect(() => {
    setSensorValue(smokeSensor.value);
    setIsSensorOverThreshold(smokeSensor.isOverTreshold);
  }, [smokeSensor]);

  useEffect(() => {
    setIsSensorEnabled(smokeSensorSettings.isEnabled);
    setSensorTreshold(smokeSensorSettings.threshold);
    setSensorPin(smokeSensorSettings.pin);
  }, [smokeSensorSettings]);

  const handleSensorPinChange = (e) => {
    const newPin = parseInt(e.target.value);
    const oldPin = parseInt(sensorPin);

    const success = handlePinChange(oldPin, newPin);

    if (success) {
      setSensorPin(newPin);
    }
  };

  const handleSmokeSensorSettingsSave = () => {
    setSmokeSensorSettings(isSensorEnabled, sensorTreshold, sensorPin);
  }

  return (
    <SettingsForm title='Детектор диму'>
      <WrapperBlock>
        <DarkWrapperBlock className="justify-between">
          <p className="text-gray-300">
            Значення: <span className={isSensorOverThreshold ? "text-red-700" : ""}>{sensorValue === -1 ? "Ініціалізація" : sensorValue}</span>
          </p>
        </DarkWrapperBlock>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label='Увімкнений'>
            <SelectToggle
              value={isSensorEnabled}
              onChange={(e) => setIsSensorEnabled(e.target.value)}
              className='w-full'
            />
          </FormField>
          <FormField label='Поріг спрацювання'>
            <Input className='w-full' value={sensorTreshold} onChange={(e) => setSensorTreshold(e.target.value)} />
          </FormField>
          <FormField label='Пін'>
            <Select
              value={sensorPin}
              options={getAvailableInputPins(sensorPin)}
              onChange={handleSensorPinChange}
              className={'w-full'}
            />
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>
      <SaveButton onClick={handleSmokeSensorSettingsSave}/>
    </SettingsForm>
  )
}

export default SmokeSensor