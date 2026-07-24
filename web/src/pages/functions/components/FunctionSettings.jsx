import ColumnBlock from '@/components/layout/ColumnBlock';
import Select from '@/components/ui/Select';
import SaveButton from '@/components/ui/SaveButton';
import Button from '@/components/ui/Button';
import { FunctionForm } from './FunctionForm';
import { useFunctionEditor } from '../hooks/useFunctionEditor';
import { useFormOptions } from '../hooks/useFormOptions';

export const FunctionSettings = () => {
  const {
    functionList,
    selectedIndex,
    selectedFunction,
    isEdited,
    updateFunction,
    handleFunctionSelect,
    handleSave,
    handleDelete,
  } = useFunctionEditor();

  const options = useFormOptions();

  return (
    <ColumnBlock>
      <div className="flex items-center justify-between gap-2">
        <h2 className="font-semibold text-2xl">Налаштування функцій</h2>
        <div className="flex items-center gap-2">
          {isEdited && (
            <span className="text-xs text-yellow-500 font-medium">Незбережені зміни</span>
          )}
          {selectedFunction && (
            <span className={`text-xs font-semibold px-2 py-0.5 rounded ${selectedFunction.isActive ? 'bg-green-700 text-green-100' : 'bg-gray-700 text-gray-400'}`}>
              {selectedFunction.isActive ? 'активна' : 'неактивна'}
            </span>
          )}
        </div>
      </div>

      <Select
        value={selectedIndex}
        options={[
          ...functionList.map((f, i) => ({
            value: i,
            text: f.name || `Функція ${i}`,
          })),
          { value: functionList.length, text: '+ Нова функція' },
        ]}
        color="light-gray"
        onChange={handleFunctionSelect}
        className="w-full"
      />

      <FunctionForm
        selectedFunction={selectedFunction}
        options={options}
        onUpdateFunction={updateFunction}
      />

      <div className="flex flex-col gap-2">
        <SaveButton onClick={handleSave} />
        <Button buttonText="Видалити" color="red" onClick={handleDelete} />
      </div>
    </ColumnBlock>
  );
};