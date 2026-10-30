import Input from '@/components/ui/Input';
import FormField from '@/components/ui/FormField';
import SelectToggle from '@/components/ui/SelectToggle';
import DarkWrapperBlock from '@/components/layout/DarkWrapperBlock';
import { ConditionGroup } from './ConditionTree';
import { ActionList } from './ActionList';
import { EMPTY_ACTION, toEditorTree } from '../utils/conditionTree';
import { describeFunction } from '../utils/describe';

const SectionTitle = ({ children }) => (
  <h3 className='text-sm font-semibold text-gray-400 uppercase tracking-wider'>{children}</h3>
);

export const FunctionForm = ({ selectedFunction, options, onUpdateFunction }) => {
  if (!selectedFunction) return null;

  const actions = selectedFunction.actions || [];

  const handleActionChange = (index, updates) => {
    const newActions = [...actions];
    newActions[index] = { ...newActions[index], ...updates };
    onUpdateFunction({ actions: newActions });
  };

  const handleAddAction = () => {
    onUpdateFunction({ actions: [...actions, { ...EMPTY_ACTION }] });
  };

  const handleRemoveAction = (index) => {
    const newActions = actions.filter((_, i) => i !== index);
    onUpdateFunction({ actions: newActions.length ? newActions : [{ ...EMPTY_ACTION }] });
  };

  return (
    <div className='flex flex-col gap-4'>
      {/* Function metadata */}
      <DarkWrapperBlock className='!flex-col !items-stretch gap-3'>
        <FormField label='Назва'>
          <Input
            value={selectedFunction.name}
            onChange={(e) => onUpdateFunction({ name: e.target.value })}
            placeholder='Введіть назву'
            className='w-full'
          />
        </FormField>
        <div className='flex gap-4'>
          <FormField label='Увімкнено'>
            <SelectToggle
              value={selectedFunction.isEnabled}
              onChange={(e) => onUpdateFunction({ isEnabled: e.target.value === 'true' })}
            />
          </FormField>
          <FormField label='Пріоритет'>
            <Input
              value={selectedFunction.priority ?? 0}
              isNumber
              onChange={(e) => onUpdateFunction({ priority: parseInt(e.target.value) || 0 })}
              className='w-20'
            />
            <p className='mt-1 text-xs text-gray-500'>при конфлікті за реле перемагає більший</p>
          </FormField>
        </div>
      </DarkWrapperBlock>

      {/* Whole rule in one sentence */}
      <p className='text-sm text-gray-300 bg-gray-900 border border-gray-700 rounded px-3 py-2 break-words'>
        {describeFunction(selectedFunction.conditionTree, actions)}
      </p>

      <div className='flex flex-col gap-2'>
        <SectionTitle>Якщо</SectionTitle>
        <ConditionGroup
          node={selectedFunction.conditionTree}
          options={options}
          onChange={(newTree) => onUpdateFunction({ conditionTree: newTree ?? toEditorTree([]) })}
        />
      </div>

      <div className='flex flex-col gap-2'>
        <SectionTitle>То</SectionTitle>
        <ActionList
          actions={actions}
          options={options}
          onActionChange={handleActionChange}
          onAddAction={handleAddAction}
          onRemoveAction={handleRemoveAction}
        />
        <p className='text-xs text-gray-500'>
          Коли умова не виконується, функція не керує реле: ним керують інші функції, а якщо жодна — реле в режимі АВТО вимкнене.
        </p>
      </div>
    </div>
  );
};
