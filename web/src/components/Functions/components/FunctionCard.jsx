import Button from '@/components/ui/Button';
import DarkWrapperBlock from '@/components/layout/DarkWrapperBlock';
import useFunctionStore from '@/store/functionStore';
import { toEditorTree } from '@/pages/functions/utils/conditionTree';
import { describeFunction } from '@/pages/functions/utils/describe';

const FunctionCard = ({ functionData, functionIndex }) => {
  const { setFunctionIsEnabled } = useFunctionStore();

  const handleFunctionToggle = () => {
    setFunctionIsEnabled(functionIndex, !functionData.isEnabled);
  };

  return (
    <DarkWrapperBlock className='justify-between !flex-col xl:!flex-row !items-start xl:!items-center gap-4'>
      <div className="w-full">
        <p className="text-lg font-bold text-white mb-1 break-words">
          Функція {functionIndex}: {functionData.name || '(Без назви)'}
        </p>
        <p className="text-sm text-gray-400 mb-1 break-words">
          {describeFunction(toEditorTree(functionData.conditions), functionData.actions)}
        </p>
        <p className="text-sm text-gray-500 mb-2">Пріоритет: {functionData.priority ?? 0}</p>
        
        <div className="flex gap-2">
          <p className="text-md text-gray-300">
            Стан: <span className={functionData.isEnabled ? "text-green-400" : "text-red-400"}>{functionData.isEnabled ? 'Увімкнено' : 'Вимкнено'}</span>,
            {' '}Активна: <span className={functionData.isActive ? "text-green-400" : "text-gray-400"}>{functionData.isActive ? 'Так' : 'Ні'}</span>
          </p>
        </div>
      </div>
      <Button
        buttonText={functionData.isEnabled ? 'Вимкнути' : 'Увімкнути'}
        color={functionData.isEnabled ? 'red' : 'green'}
        onClick={handleFunctionToggle}
        className="w-full xl:w-auto shrink-0"
      />
    </DarkWrapperBlock>
  );
};

export default FunctionCard;