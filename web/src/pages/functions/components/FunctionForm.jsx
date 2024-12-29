import Select from '@/components/ui/Select';
import Input from '@/components/ui/Input';
import DarkWrapperBlock from '@/components/layout/DarkWrapperBlock';

const ParameterRow = ({ label, tempValue, tempOptions, onTempChange, signValue, signOptions, onSignChange, deltaValue, onDeltaChange }) => (
  <div className='flex flex-col'>
    <p className='text-lg text-gray-300'>{label}:</p>
    <div className='flex gap-2'>
      <Select
        value={tempValue}
        options={tempOptions}
        onChange={onTempChange}
      />
      <Select
        value={signValue}
        options={signOptions}
        onChange={onSignChange}
      />
      <Input
        placeholder='ΔT'
        className='w-12'
        maxLength={3}
        value={deltaValue}
        isNumber={true}
        onChange={onDeltaChange}
      />
    </div>
  </div>
);

export const FunctionForm = ({
  selectedFunction,
  tempSensorsOptions,
  deltaValueSignOptions,
  signOptions,
  relayOptions,
  onTemperatureChange,
  onDeltaValueChange,
  onDeltaSignChange,
  onSignChange,
  onRelayChange,
}) => {
  return (
    <DarkWrapperBlock className='gap-4 md:!flex-col !items-start'>
      <div className='flex gap-2 flex-col self-center'>
        <ParameterRow
          label="Параметр 1"
          tempValue={selectedFunction?.temperatureIndex[0]}
          tempOptions={tempSensorsOptions}
          onTempChange={onTemperatureChange(0)}
          signValue={selectedFunction?.deltaValueSign[0]}
          signOptions={deltaValueSignOptions}
          onSignChange={onDeltaSignChange(0)}
          deltaValue={selectedFunction?.deltaValue[0]}
          onDeltaChange={onDeltaValueChange(0)}
        />

        <div className='flex flex-col'>
          <p className='text-lg text-gray-300'>Знак:</p>
          <Select
            value={selectedFunction?.sign}
            options={signOptions}
            onChange={onSignChange}
          />
        </div>

        <ParameterRow
          label="Параметр 2"
          tempValue={selectedFunction?.temperatureIndex[1]}
          tempOptions={tempSensorsOptions}
          onTempChange={onTemperatureChange(1)}
          signValue={selectedFunction?.deltaValueSign[1]}
          signOptions={deltaValueSignOptions}
          onSignChange={onDeltaSignChange(1)}
          deltaValue={selectedFunction?.deltaValue[1]}
          onDeltaChange={onDeltaValueChange(1)}
        />

        <div className='flex flex-col'>
          <p className='text-lg text-gray-300'>Реле:</p>
          <Select
            value={selectedFunction?.relayIndex}
            options={relayOptions}
            onChange={onRelayChange}
          />
        </div>
      </div>
    </DarkWrapperBlock>
  );
};