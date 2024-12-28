import DarkWrapperBlock from '../../../components/layout/DarkWrapperBlock';
import FormField from '../../../components/ui/FormField';
import Select from '../../../components/ui/Select';
import SaveButton from '../../../components/ui/SaveButton';
import usePinStore from '../../../store/pinStore';

export const RelayConfigList = ({ relays, onPinChange, onActiveHighChange, onSave }) => {
  const { getAvailableOutputPins } = usePinStore();

  return (
    <div className="flex flex-col gap-2">
      <div className="grid grid-cols-1 md:grid-cols-2 gap-2">
        {relays?.map((relay, index) => (
          <DarkWrapperBlock className="md:!flex-col !items-start" key={index}>
            <p className="text-lg text-gray-300">
              Реле {index}:
            </p>
            <FormField label='Пін'>
              <Select
                value={relay.pin}
                options={getAvailableOutputPins(relay.pin)}
                onChange={onPinChange(index)}
                className='w-full'
              />
            </FormField>
            <FormField label='Увімкнено при'>
              <Select
                value={relay.isActiveOnHigh}
                options={[
                  { value: 0, text: 'Low' },
                  { value: 1, text: 'High' },
                ]}
                onChange={onActiveHighChange(index)}
                className='w-full'
              />
            </FormField>
          </DarkWrapperBlock>
        ))}
      </div>
      <SaveButton onClick={onSave} />
    </div>
  );
};