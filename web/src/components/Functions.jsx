import Button from "./ui/Button";
import DarkWrapperBlock from "./layout/DarkWrapperBlock";
import ColumnBlock from "./layout/ColumnBlock";
import useFunctionStore from "../store/functionStore";

const Functions = ({ isTwoCols = false }) => {
  const { functions, tnIndex, tUnknownIndex, relayUnknownIndex, setFunctionIsEnabled } = useFunctionStore();

  const getTemperatureText = (customFunction, temperatureIndex) => {
    let result = "";
    if (customFunction.temperatureIndex[temperatureIndex] !== tnIndex)
      result = customFunction.temperatureIndex[temperatureIndex] !== tUnknownIndex ? `T${customFunction.temperatureIndex[temperatureIndex]}` : 'Н/Д';

    if (customFunction.deltaValue[temperatureIndex] !== 0) {
      result += customFunction.deltaValueSign[temperatureIndex] ? customFunction.temperatureIndex[temperatureIndex] === tnIndex ? '' : ' + ' : ' - ';
      result += customFunction.deltaValue[temperatureIndex];
    }

    return result;
  };

  const getCustomFunctionText = (customFunction) => {
    const temperatureText1 = getTemperatureText(customFunction, 0);
    const temperatureText2 = getTemperatureText(customFunction, 1);
    let customSign = '<';

    switch (customFunction.sign) {
      case 1:
        customSign = '=';
        break;
      case 2:
        customSign = '>';
        break;
    }

    return `${temperatureText1} ${customSign} ${temperatureText2}`;
  }
  
  const handleFunctionToggle = (index) => () => {
    setFunctionIsEnabled(index, !functions[index].isEnabled);
  }

  return (
    <ColumnBlock>
      <h2 className="font-semibold text-2xl">Функції</h2>
      <div className={isTwoCols ? "grid grid-cols-1 lg:grid-cols-2 gap-4" : "flex flex-col gap-4"}>
        {functions?.map((customFunction, index) => (
          <DarkWrapperBlock className='justify-between' key={index}>
            <div>
              <p className="text-lg text-gray-300">
                Функція {index}: {getCustomFunctionText(customFunction)}
              </p>
              <p className="text-lg text-gray-300">
                Реле: {customFunction.relayIndex !== relayUnknownIndex ? `Реле ${customFunction.relayIndex}` : 'Н/Д'}
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
              onClick={handleFunctionToggle(index)}
            />
          </DarkWrapperBlock>
        ))}
      </div>
    </ColumnBlock>
  )
}

export default Functions