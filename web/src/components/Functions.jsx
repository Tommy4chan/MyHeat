import Button from "./ui/buttons/Button";
import DarkWrapperBlock from "./ui/DarkWrapperBlock";
import ColumnBlock from "./ui/ColumnBlock";
import useFunctionStore from "../store/functionStore";

const Functions = () => {
  const { functions } = useFunctionStore();

  const tnIndex = 255;

  const getTemperatureText = (customFunctionIndex, temperatureIndex) => {
    let result = "";
    if (functions[customFunctionIndex].temperatureIndex[temperatureIndex] !== tnIndex)
      result = `T${functions[customFunctionIndex].temperatureIndex[temperatureIndex]}`;

    if (functions[customFunctionIndex].deltaValue[temperatureIndex] !== 0) {
      result += functions[customFunctionIndex].deltaValueSign[temperatureIndex] ? functions[customFunctionIndex].temperatureIndex[temperatureIndex] === tnIndex ? '' : ' + ' : ' - ';
      result += functions[customFunctionIndex].deltaValue[temperatureIndex];
    }

    return result;
  };

  const getCustomFunctionText = (customFunctionIndex) => {
    const temperatureText1 = getTemperatureText(customFunctionIndex, 0);
    const temperatureText2 = getTemperatureText(customFunctionIndex, 1);
    let customSign = '<';

    switch (functions[customFunctionIndex].sign) {
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
        {functions?.map((customFunction, index) => (
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
            <Button
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