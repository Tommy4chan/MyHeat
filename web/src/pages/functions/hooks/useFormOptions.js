import { useMemo } from 'react';
import useFunctionStore from '@/store/functionStore';

export const useFormOptions = () => {
  const { 
    tnIndex, 
    tUnknownIndex, 
    relayUnknownIndex, 
    temperatureCount, 
    relayCount 
  } = useFunctionStore();

  const tempSensorsOptions = useMemo(
    () => [
      ...Array.from({ length: temperatureCount }, (_, i) => ({ value: i, text: `T${i}` })),
      { value: tnIndex, text: 'TN' },
      { value: tUnknownIndex, text: 'Н/Д', isDisabled: true }
    ],
    [temperatureCount, tnIndex, tUnknownIndex]
  );

  const relayOptions = useMemo(
    () => [
      ...Array.from({ length: relayCount }, (_, i) => ({ value: i, text: `Реле ${i}` })),
      { value: relayUnknownIndex, text: 'Н/Д', isDisabled: true }
    ],
    [relayCount, relayUnknownIndex]
  );

  const deltaValueSignOptions = [
    { value: 0, text: '-' },
    { value: 1, text: '+' }
  ];

  const signOptions = [
    { value: 0, text: '<' },
    { value: 1, text: '=' },
    { value: 2, text: '>' }
  ];

  return {
    tempSensorsOptions,
    relayOptions,
    deltaValueSignOptions,
    signOptions
  };
};