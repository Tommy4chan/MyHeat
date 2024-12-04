import { useState } from 'react';
import usePinStore from '../../store/pinStore';
import { handlePinChange } from '../../utils/pinHandler';
import Select from '../ui/Select';
import DarkWrapperBlock from '../ui/DarkWrapperBlock';
import FormColumn from '../ui/FormColumn';
import SaveButton from '../ui/buttons/SaveButton';
import FormField from '../ui/FormField';

const Temperature = () => {
  const { 
    getAvailableInputPins,
  } = usePinStore();

  const [temperaturePin, setTemperaturePin] = useState(21);

  const handleTemperaturePinChange = (e) => {
    handlePinChange(e, setTemperaturePin, temperaturePin);
  }

  return (
    <FormColumn title='Дачики температури'>
      <div className='flex flex-col gap-2 w-full'>
        <DarkWrapperBlock className="md:!flex-col">
        <FormField label='Пін'>
            <Select
              value={temperaturePin}
              options={getAvailableInputPins(temperaturePin)}
              onChange={handleTemperaturePinChange}
              className='w-full'
            />
          </FormField>
        </DarkWrapperBlock>
      </div>
      <SaveButton />
    </FormColumn>
  )
}

export default Temperature