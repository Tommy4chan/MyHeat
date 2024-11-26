import { useEffect, useState } from "react";
import BasicButton from "./ui/buttons/BasicButton";
import DarkWrapperBlock from "./ui/DarkWrapperBlock";
import ColumnBlock from "./ui/ColumnBlock";

const Functions = () => {
  const [customFunctions, setCustomFunctions] = useState([]);
  const maxTemperatureSensors = 4;

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
      deltaValueSign: [0, 0],
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

  const getTemperatureText = (customFunctionIndex, temperatureIndex) => {
    let result = "";
    if (customFunctions[customFunctionIndex].temperatureIndex[temperatureIndex] !== maxTemperatureSensors)
      result = `T${customFunctions[customFunctionIndex].temperatureIndex[temperatureIndex]}`;

    if (customFunctions[customFunctionIndex].deltaValue[temperatureIndex] !== 0) {
      result += customFunctions[customFunctionIndex].deltaValueSign[temperatureIndex] ? customFunctions[customFunctionIndex].temperatureIndex[temperatureIndex] === maxTemperatureSensors ? '' : ' + ' : ' - ';
      result += customFunctions[customFunctionIndex].deltaValue[temperatureIndex];
    }

    return result;
  };

  const getCustomFunctionText = (customFunctionIndex) => {
    const temperatureText1 = getTemperatureText(customFunctionIndex, 0);
    const temperatureText2 = getTemperatureText(customFunctionIndex, 1);
    let customSign = '<';

    switch (customFunctions[customFunctionIndex].sign) {
      case 1:
        customSign = '=';
      case 2:
        customSign = '>';
    }

    return `${temperatureText1} ${customSign} ${temperatureText2}`;

  }

  return (
    <ColumnBlock>
      <h2 className="font-semibold text-2xl">Функції</h2>
      <div className="flex flex-col gap-4">
        {customFunctions?.map((customFunction, index) => (
          <DarkWrapperBlock className='justify-between' key={index}>
            <div>
              <p className="text-lg text-gray-300">
                Функція {index}: {getCustomFunctionText(index)}
              </p>
              <p className="text-lg text-gray-300">
                Реле: Реле {customFunction.relayIndex}
              </p>
              <div className="flex justify-between gap-2">
                <p className="text-lg text-gray-300">
                  Стан: {customFunction.isEnabled ? 'Увімкнено' : 'Вимкнено'},
                  Активна: {customFunction.isActive ? 'Так' : 'Ні'}
                </p>
              </div>
            </div>
            <BasicButton
              buttonText={customFunction.isEnabled ? 'Вимкнути' : 'Увімкнути'}
              color={customFunction.isEnabled ? 'red' : 'green'}
            />
          </DarkWrapperBlock>
        ))}
      </div>
    </ColumnBlock>
  )
}

export default Functions