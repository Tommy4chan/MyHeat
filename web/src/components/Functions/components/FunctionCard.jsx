import Button from '@/components/ui/Button';
import DarkWrapperBlock from '@/components/layout/DarkWrapperBlock';
import { useFunction } from '../hooks/useFunction';
import useFunctionStore from '@/store/functionStore';

const FunctionCard = ({ functionData, functionIndex }) => {
  const { relayUnknownIndex, setFunctionIsEnabled } = useFunctionStore();
  const { getCustomFunctionText } = useFunction();

  const handleFunctionToggle = () => {
    setFunctionIsEnabled(functionIndex, !functionData.isEnabled);
  };

  return (
    <DarkWrapperBlock className='justify-between'>
      <div>
        <p className="text-lg text-gray-300">
          Функція {functionIndex}: {getCustomFunctionText(functionData)}
        </p>
        <p className="text-lg text-gray-300">
          Реле: {functionData.relayIndex !== relayUnknownIndex
            ? `Реле ${functionData.relayIndex}`
            : 'Н/Д'}
        </p>
        <div className="flex justify-between gap-2">
          <p className="text-lg text-gray-300">
            Стан: {functionData.isEnabled ? 'Увімкнено' : 'Вимкнено'},
            Активна: {functionData.isActive ? 'Так' : 'Ні'}
          </p>
        </div>
      </div>
      <Button
        buttonText={functionData.isEnabled ? 'Вимкнути' : 'Увімкнути'}
        color={functionData.isEnabled ? 'red' : 'green'}
        onClick={handleFunctionToggle}
      />
    </DarkWrapperBlock>
  );
};

export default FunctionCard;