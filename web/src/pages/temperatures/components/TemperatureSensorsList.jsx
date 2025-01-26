import React from 'react';
import WrapperBlock from '@/components/layout/WrapperBlock';
import DarkWrapperBlock from '@/components/layout/DarkWrapperBlock';
import Select from '@/components/ui/Select';
import SaveButton from '@/components/ui/SaveButton';
import Button from '@/components/ui/Button';
import { useTemperatureSensors } from '../hooks/useTemperatureSensors';

export const TemperatureSensorsList = () => {
  const {
    temperatureSensors,
    temperatureOptions,
    handleSensorTempIndexChange,
    handleSensorSave,
    getDiscoveredTemperatureSensors,
  } = useTemperatureSensors();
  
  return (
    <WrapperBlock>
      <h3 className='text-xl'>Датчики:</h3>
      {temperatureSensors.length !== 0 ? temperatureSensors.map((temperatureSensor, index) => (
        <DarkWrapperBlock className="justify-between" key={temperatureSensor.id}>
          <p className="text-lg text-gray-300">
            Датчик {index}: {temperatureSensor.address}
          </p>
          <div className="flex gap-2 flex-col md:flex-row">
            <Select
              value={temperatureSensor.tempIndex}
              options={temperatureOptions}
              onChange={handleSensorTempIndexChange(temperatureSensor.id)}
            />
            <SaveButton onClick={handleSensorSave(index)} />
          </div>
        </DarkWrapperBlock>
      )) :
        <p>Не знайдено</p>
      }
      <Button 
        buttonText={'Виявити датчики'} 
        color="indigo" 
        onClick={getDiscoveredTemperatureSensors} 
      />
    </WrapperBlock>
  );
};