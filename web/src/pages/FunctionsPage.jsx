import { useEffect, useMemo, useState } from 'react';
import Functions from '../components/Functions'
import ColumnBlock from '../components/ui/ColumnBlock'
import Select from '../components/ui/Select';
import DarkWrapperBlock from '../components/ui/DarkWrapperBlock';
import Button from '../components/ui/buttons/Button';
import Input from '../components/ui/Input';
import SaveButton from '../components/ui/buttons/SaveButton';
import useFunctionStore from '../store/functionStore';
import WrapperBlock from '../components/ui/WrapperBlock';
import { use } from 'react';

const FunctionsPage = () => {
  const [customFunctions, setCustomFunctions] = useState([]);
  const [selectedCustomFunction, setSelectedCustomFunction] = useState(0);

  const { functions, tnIndex, tUnknownIndex, relayUnknownIndex, temperatureCount, relayCount, emptyCustomFunction } = useFunctionStore();

  useEffect(() => {
    setCustomFunctions(functions);

    if (functions.length === 0) {
      setCustomFunctions([emptyCustomFunction]);
    }
  }, []);

  const tempSensorsOptions = useMemo(
    () => [
      ...Array.from({ length: temperatureCount }, (_, i) => ({ value: i, text: `T${i}` })),
      { value: tnIndex, text: 'TN' },
      { value: tUnknownIndex, text: 'Н/Д', isDisabled: true }
    ],
    [temperatureCount]
  );

  const relayOptions = useMemo(
    () => [
      ...Array.from({ length: relayCount }, (_, i) => ({ value: i, text: `Реле ${i}` })),
      { value: relayUnknownIndex, text: 'Н/Д', isDisabled: true }
    ],
    [relayCount]
  );

  const deltaValueSignOptions = [
    { value: 0, text: '-' },
    { value: 1, text: '+' }
  ];

  const handleCustomFunctionChange = (e) => {
    if (+e.target.value === customFunctions.length) {
      setCustomFunctions([...customFunctions, emptyCustomFunction]);
    }

    setSelectedCustomFunction(e.target.value);
  }

  const handleCustomFunctionTemperatureChange = (e, temperatureIndex) => {
    const newCustomFunctions = [...customFunctions];
    newCustomFunctions[selectedCustomFunction].temperatureIndex[temperatureIndex] = e.target.value;
    setCustomFunctions(newCustomFunctions);
  }

  const handleCustomFunctionDeltaValueChange = (e, deltaValueIndex) => {
    const newCustomFunctions = [...customFunctions];

    e.target.value = Math.abs(e.target.value);
    newCustomFunctions[selectedCustomFunction].deltaValue[deltaValueIndex] = e.target.value;

    setCustomFunctions(newCustomFunctions);
  }

  const handleCustomFunctionDeltaValueSignChange = (e, deltaValueIndex) => {
    const newCustomFunctions = [...customFunctions];
    newCustomFunctions[selectedCustomFunction].deltaValueSign[deltaValueIndex] = e.target.value;
    setCustomFunctions(newCustomFunctions);
  }

  const handleCustomFunctionsSave = () => {
    useFunctionStore.getState().setFunctionsSettings(customFunctions);
  }

  const handleCustomFunctionsDelete = () => {
    let newCustomFunctions = [...customFunctions];

    if (customFunctions.length === 1) {
      newCustomFunctions = [emptyCustomFunction];
    }
    else {
      newCustomFunctions.splice(selectedCustomFunction, 1);
      setSelectedCustomFunction(+selectedCustomFunction === 0 ? 0 : selectedCustomFunction - 1);
    }

    setCustomFunctions(newCustomFunctions);
    useFunctionStore.getState().setFunctionsSettings(newCustomFunctions);
  }

  return (
    <div className="grid grid-cols-1 lg:grid-cols-3 items-start gap-6">
      <div className="col-span-2">
        <Functions isTwoCols={true} />
      </div>

      <ColumnBlock>
        <h2 className="font-semibold text-2xl">Налаштування Функцій</h2>
        <Select
          value={selectedCustomFunction}
          options={[
            ...Array.from({ length: customFunctions.length }, (_, i) => ({ value: i, text: `Функція ${i}` })),
            { value: customFunctions.length, text: 'Створити' }
          ]}
          color='light-gray'
          onChange={(e) => handleCustomFunctionChange(e)}
        />
        <DarkWrapperBlock className='gap-4 md:!flex-col !items-start'>
          <div className='flex gap-2 flex-col self-center'>
            <div className='flex flex-col'>
              <p className='text-lg text-gray-300'>Параметр 1:</p>
              <div className='flex gap-2'>
                <Select
                  value={customFunctions[selectedCustomFunction]?.temperatureIndex[0]}
                  options={tempSensorsOptions}
                  onChange={(e) => handleCustomFunctionTemperatureChange(e, 0)}
                />
                <Select
                  value={customFunctions[selectedCustomFunction]?.deltaValueSign[0]}
                  options={deltaValueSignOptions}
                  onChange={(e) => handleCustomFunctionDeltaValueSignChange(e, 0)}
                />
                <Input
                  placeholder='ΔT'
                  className='w-12'
                  maxLength={3}
                  value={customFunctions[selectedCustomFunction]?.deltaValue[0]}
                  isNumber={true}
                  onChange={(e) => handleCustomFunctionDeltaValueChange(e, 0)}
                />
              </div>
            </div>
            <div className='flex flex-col'>
              <p className='text-lg text-gray-300'>Знак:</p>
              <Select
                value={customFunctions[selectedCustomFunction]?.sign}
                options={[
                  { value: 0, text: '<' },
                  { value: 1, text: '=' },
                  { value: 2, text: '>' }
                ]}
                onChange={(e) => {
                  const newCustomFunctions = [...customFunctions];
                  newCustomFunctions[selectedCustomFunction].sign = e.target.value;
                  setCustomFunctions(newCustomFunctions);
                }}
              />
            </div>
            <div className='flex flex-col'>
              <p className='text-lg text-gray-300'>Параметр 2:</p>
              <div className='flex gap-2'>
                <Select
                  value={customFunctions[selectedCustomFunction]?.temperatureIndex[1]}
                  options={tempSensorsOptions}
                  onChange={(e) => handleCustomFunctionTemperatureChange(e, 1)}
                />
                <Select
                  value={customFunctions[selectedCustomFunction]?.deltaValueSign[1]}
                  options={deltaValueSignOptions}
                  onChange={(e) => handleCustomFunctionDeltaValueSignChange(e, 1)}
                />
                <Input
                  placeholder='ΔT'
                  className='w-12'
                  maxLength={3}
                  value={Math.abs(customFunctions[selectedCustomFunction]?.deltaValue[1])}
                  isNumber={true}
                  onChange={(e) => handleCustomFunctionDeltaValueChange(e, 1)}
                />
              </div>
            </div>
            <div className='flex flex-col'>
              <p className='text-lg text-gray-300'>Реле:</p>
              <Select
                value={customFunctions[selectedCustomFunction]?.relayIndex}
                options={relayOptions}
                onChange={(e) => {
                  const newCustomFunctions = [...customFunctions];
                  newCustomFunctions[selectedCustomFunction].relayIndex = e.target.value;
                  setCustomFunctions(newCustomFunctions);
                }}
              />
            </div>
          </div>
        </DarkWrapperBlock>
        <WrapperBlock>
          <SaveButton
            onClick={handleCustomFunctionsSave}
          />
          <Button
            buttonText='Видалити'
            color='red'
            onClick={handleCustomFunctionsDelete}
          />
        </WrapperBlock>

      </ColumnBlock>
    </div>
  )
}

export default FunctionsPage