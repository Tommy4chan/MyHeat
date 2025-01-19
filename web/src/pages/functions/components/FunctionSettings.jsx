import ColumnBlock from '@/components/layout/ColumnBlock';
import Select from '@/components/ui/Select';
import WrapperBlock from '@/components/layout/WrapperBlock';
import SaveButton from '@/components/ui/SaveButton';
import Button from '@/components/ui/Button';
import { FunctionForm } from './FunctionForm';
import { useFunctionEditor } from '../hooks/useFunctionEditor';
import { useFormOptions } from '../hooks/useFormOptions';

export const FunctionSettings = () => {  
  const {
    functionList,
    selectedIndex,
    selectedFunction,
    handleFunctionSelect,
    handleTemperatureChange,
    handleDeltaValueChange,
    handleDeltaSignChange,
    handleSignChange,
    handleRelayChange,
    handleSave,
    handleDelete,
  } = useFunctionEditor();

  const {
    tempSensorsOptions,
    relayOptions,
    deltaValueSignOptions,
    signOptions
  } = useFormOptions();

  return (
    <ColumnBlock>
      <h2 className="font-semibold text-2xl">Налаштування Функцій</h2>
      <Select
        value={selectedIndex}
        options={[
          ...functionList.map((_, i) => ({ value: i, text: `Функція ${i}` })),
          { value: functionList.length, text: 'Створити' }
        ]}
        color='light-gray'
        onChange={handleFunctionSelect}
      />
      
      <FunctionForm
        selectedFunction={selectedFunction}
        tempSensorsOptions={tempSensorsOptions}
        deltaValueSignOptions={deltaValueSignOptions}
        signOptions={signOptions}
        relayOptions={relayOptions}
        onTemperatureChange={handleTemperatureChange}
        onDeltaValueChange={handleDeltaValueChange}
        onDeltaSignChange={handleDeltaSignChange}
        onSignChange={handleSignChange}
        onRelayChange={handleRelayChange}
      />

      <WrapperBlock>
        <SaveButton onClick={handleSave} />
        <Button
          buttonText='Видалити'
          color='red'
          onClick={handleDelete}
        />
      </WrapperBlock>
    </ColumnBlock>
  );
};