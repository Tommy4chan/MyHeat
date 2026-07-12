import DarkWrapperBlock from '@/components/layout/DarkWrapperBlock';
import Input from '@/components/ui/Input';
import SettingsForm from '@/components/layout/SettingsForm';
import SaveButton from '@/components/ui/SaveButton';
import FormField from '@/components/ui/FormField';
import SelectToggle from "@/components/ui/SelectToggle";
import { useMqttSettings } from '../hooks/useMqttSettings';

const Mqtt = () => {
  const {
    settings,
    handleSettingsChange,
    handleMqttSettingsSave,
  } = useMqttSettings();

  return (
    <SettingsForm title='MQTT'>
      <DarkWrapperBlock className='md:!flex-col'>
        <FormField label="Увімкнено">
          <SelectToggle
            value={settings.isEnabled}
            onChange={handleSettingsChange}
            className='w-full'
            name='isEnabled'
          />
        </FormField>
        <FormField label="Брокер">
          <Input className='w-full' value={settings.broker || ''} onChange={handleSettingsChange} name='broker' placeholder="192.168.1.100" />
        </FormField>
        <FormField label="Порт">
          <Input type="number" className='w-full' value={settings.port || 1883} onChange={handleSettingsChange} name='port' />
        </FormField>
        <FormField label="Користувач">
          <Input className='w-full' value={settings.user || ''} onChange={handleSettingsChange} name='user' />
        </FormField>
        <FormField label="Пароль">
          <Input className='w-full' value={settings.password || ''} onChange={handleSettingsChange} name='password' />
        </FormField>
        <FormField label="Інтервал оновлення (мс)">
          <Input type="number" className='w-full' value={settings.publishInterval || 60000} onChange={handleSettingsChange} name='publishInterval' />
        </FormField>
        <FormField label="Статус">
          <div className="flex items-center">
            {settings.isConnected ? (
                <span className="text-green-500 font-bold">Підключено</span>
            ) : (
                <span className="text-red-500 font-bold">Відключено</span>
            )}
          </div>
        </FormField>
      </DarkWrapperBlock>
      <SaveButton onClick={handleMqttSettingsSave} />
    </SettingsForm>
  )
}

export default Mqtt;
