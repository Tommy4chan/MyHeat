import Input from "@/components/ui/Input";
import DarkWrapperBlock from "@/components/layout/DarkWrapperBlock";
import SaveButton from "@/components/ui/SaveButton";
import SettingsForm from "@/components/layout/SettingsForm";
import FormField from "@/components/ui/FormField";
import SelectToggle from "@/components/ui/SelectToggle";
import WrapperBlock from "@/components/layout/WrapperBlock";
import usePinStore from "@/store/pinStore";
import Select from "@/components/ui/Select";
import { useSmokeSensorSettings } from "../hooks/useSmokeSensorSettings";

const SmokeSensor = () => {

  const { getAvailableInputPins } = usePinStore();
  const { settings, sensor, handleSettingsChange, handleSmokeSensorSettingsSave } = useSmokeSensorSettings();

  return (
    <SettingsForm title='Датчик диму'>
      <WrapperBlock>
        <DarkWrapperBlock className="justify-between">
          <p className="text-gray-300">
            Значення: <span className={sensor.isOverThreshold ? "text-red-700" : ""}>{sensor.value === -1 ? "Ініціалізація" : sensor.value}</span>
          </p>
        </DarkWrapperBlock>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label='Увімкнений'>
            <SelectToggle
              value={settings.isEnabled}
              onChange={handleSettingsChange}
              className='w-full'
              name="isEnabled"
            />
          </FormField>
          <FormField label='Поріг спрацювання'>
            <Input 
              className='w-full'
              value={settings.threshold}
              onChange={handleSettingsChange}
              name="threshold"
            />
          </FormField>
          <FormField label='Пін'>
            <Select
              value={settings.pin}
              options={getAvailableInputPins(settings?.pin)}
              onChange={handleSettingsChange}
              className={'w-full'}
              name="pin"
            />
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>
      <SaveButton onClick={handleSmokeSensorSettingsSave} />
    </SettingsForm>
  )
}

export default SmokeSensor