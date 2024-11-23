import { useEffect, useState } from "react";
import BasicButton from "./ui/buttons/BasicButton";
import DarkWrapperBlock from "./ui/DarkWrapperBlock";
import ColumnBlock from "./ui/ColumnBlock";

const Functions = () => {
  const [customFunctions, setCustomFunctions] = useState([]);

  useEffect(() => {
    const customFunction = [{
      sign: 2,
      temperatureIndex: [0, 3],
      deltaValue: [-3, -20],
      relayIndex: 3,
      isEnabled: true,
      isActive: false,
    },
    {
      sign: 0,
      temperatureIndex: [2, 4],
      deltaValue: [0, 20],
      relayIndex: 1,
      isEnabled: true,
      isActive: false,
    },
    {
      sign: 1,
      temperatureIndex: [2, 1],
      deltaValue: [5, 0],
      relayIndex: 0,
      isEnabled: false,
      isActive: false,
    }];

    setCustomFunctions(customFunction);
  }, []);

  const getTemperatureText = (temperatureIndex, deltaValue) => {
    let result = `T${temperatureIndex + 1}`;

    if (deltaValue !== 0)
      result += deltaValue > 0 ? ` + ${deltaValue}` : ` - ${Math.abs(deltaValue)}`;

    return result;
  };

  const getCustomFunctionText = (customFunction) => {
    const temperatureText1 = getTemperatureText(customFunction.temperatureIndex[0], customFunction.deltaValue[0]);
    const temperatureText2 = getTemperatureText(customFunction.temperatureIndex[1], customFunction.deltaValue[1]);
    let customSign = '<';

    switch (customFunction.sign) {
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
                Функція {index}: {getCustomFunctionText(customFunction)}
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