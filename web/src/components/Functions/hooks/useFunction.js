import useFunctionStore from '@/store/functionStore';
import { useMemo } from 'react';

export const useFunction = () => {
  const { tnIndex, tUnknownIndex } = useFunctionStore();

  const getTemperatureText = (customFunction, temperatureIndex) => {
    let result = "";
    if (customFunction.temperatureIndex[temperatureIndex] !== tnIndex)
      result = customFunction.temperatureIndex[temperatureIndex] !== tUnknownIndex 
        ? `T${customFunction.temperatureIndex[temperatureIndex]}` 
        : 'Н/Д';

    if (customFunction.deltaValue[temperatureIndex] !== 0) {
      result += customFunction.deltaValueSign[temperatureIndex] 
        ? customFunction.temperatureIndex[temperatureIndex] === tnIndex ? '' : ' + ' 
        : ' - ';
      result += customFunction.deltaValue[temperatureIndex];
    }

    return result;
  };

  const getCustomFunctionText = useMemo(() => (customFunction) => {
    const temperatureText1 = getTemperatureText(customFunction, 0);
    const temperatureText2 = getTemperatureText(customFunction, 1);
    
    const signMap = {
      0: '<',
      1: '=',
      2: '>'
    };

    const customSign = signMap[customFunction.sign] || '<';

    return `${temperatureText1} ${customSign} ${temperatureText2}`;
  }, [tnIndex, tUnknownIndex]);

  return {
    getCustomFunctionText
  };
};