import React from 'react';
import WrapperBlock from '@/components/layout/WrapperBlock';
import DarkWrapperBlock from '@/components/layout/DarkWrapperBlock';
import FormField from '@/components/ui/FormField';
import Select from '@/components/ui/Select';
import Input from '@/components/ui/Input';
import SaveButton from '@/components/ui/SaveButton';
import usePinStore from '@/store/pinStore';
import { useTemperatureSettings } from '../hooks/useTemperatureSettings';

export const TemperatureSettingsForm = () => {
  const { getAvailableInputPins } = usePinStore();
  const { settings, handleSettingsChange, handleSettingsSave } = useTemperatureSettings();

  return (
    <WrapperBlock>
      <h3 className='text-xl'>Налаштування:</h3>
      <DarkWrapperBlock className="md:!flex-col">
        <FormField label='Пін'>
          <Select
            value={settings.temperaturePin}
            options={getAvailableInputPins(settings.temperaturePin)}
            onChange={handleSettingsChange}
            name='temperaturePin'
            className='w-full'
          />
        </FormField>
        <FormField label='Кількість датчиків'>
          <Input
            value={settings.temperatureCount}
            onChange={handleSettingsChange}
            isNumber={true}
            maxLength={3}
            name='temperatureCount'
            className='w-full'
          />
        </FormField>
      </DarkWrapperBlock>
      <SaveButton onClick={handleSettingsSave} />
    </WrapperBlock>
  );
};