import { useEffect, useState } from 'react';
import Functions from '../components/Functions'
import ColumnBlock from '../components/ui/ColumnBlock'
import Select from '../components/ui/Select';
import DarkWrapperBlock from '../components/ui/DarkWrapperBlock';
import Button from '../components/ui/buttons/Button';
import Input from '../components/ui/Input';
import SaveButton from '../components/ui/buttons/SaveButton';

const FunctionsPage = () => {
  const [customFunctions, setCustomFunctions] = useState([]);
  const [selectedCustomFunction, setSelectedCustomFunction] = useState(0);

  const maxTemperatureSensors = 4;
  const maxRelays = 4;

  useEffect(() => {
    const customFunction = [{
      sign: 2,
      temperatureIndex: [0, 3],
      deltaValueSign: [0, 1],
      deltaValue: [3, 20],
      relayIndex: 3,
      isEnabled: true,
      isActive: false,
    },
    {
      sign: 0,
      temperatureIndex: [2, 4],
      deltaValueSign: [0, 1],
      deltaValue: [0, 20],
      relayIndex: 1,
      isEnabled: true,
      isActive: false,
    },
    {
      sign: 1,
      temperatureIndex: [2, 1],
      deltaValueSign: [0, 1],
      deltaValue: [5, 0],
      relayIndex: 1,
      isEnabled: false,
      isActive: false,
    }];

    setCustomFunctions(customFunction);
  }, []);

  const tempSensorsOptions = [
    ...Array.from({ length: maxTemperatureSensors }, (_, i) => ({ value: i, text: `T${i}` })),
    { value: maxTemperatureSensors, text: 'TN' }
  ];

  const deltaValueSignOptions = [
    { value: 0, text: '-' },
    { value: 1, text: '+' }
  ];

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

  return (
    <div className="grid grid-cols-1 lg:grid-cols-2 items-start gap-6">
      <Functions />
      <ColumnBlock>
        <h2 className="font-semibold text-2xl">Налаштування Функції</h2>
        <Select
          value={selectedCustomFunction}
          options={[
            ...Array.from({ length: customFunctions.length }, (_, i) => ({ value: i, text: `Функція ${i}` })),
            { value: customFunctions.length, text: 'Створити' }
          ]}
          color='light-gray'
          onChange={(e) => setSelectedCustomFunction(e.target.value)}
        />
        <div>
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
                  options={[
                    ...Array.from({ length: maxRelays }, (_, i) => ({ value: i, text: `Реле ${i}` }))
                  ]}
                  onChange={(e) => {
                    const newCustomFunctions = [...customFunctions];
                    newCustomFunctions[selectedCustomFunction].relayIndex = e.target.value;
                    setCustomFunctions(newCustomFunctions);
                  }}
                />
              </div>
            </div>

          </DarkWrapperBlock>
        </div>
        <SaveButton />
      </ColumnBlock>
    </div>
  )
}

export default FunctionsPage