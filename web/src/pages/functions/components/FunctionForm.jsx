import Input from '@/components/ui/Input';
import FormField from '@/components/ui/FormField';
import SelectToggle from '@/components/ui/SelectToggle';
import DarkWrapperBlock from '@/components/layout/DarkWrapperBlock';
import { ConditionNode } from './ConditionTree';
import { ActionList } from './ActionList';
import { ACT_SET_RELAY, COND_CMP, SRC_TEMP, OP_LT } from '@/utils/functionEnums';

const EMPTY_CMP = {
  t: COND_CMP, src: SRC_TEMP, si: 0,
  d1: 0, d1sign: 1,
  op: OP_LT,
  ivs: false, v: 25,
  src2: SRC_TEMP, si2: 0,
  d2: 0, d2sign: 1,
  hyst: 0.5,
  children: [],
};

export const FunctionForm = ({ selectedFunction, options, onUpdateFunction }) => {
  if (!selectedFunction) return null;

  const handleActionChange = (index, field, value) => {
    const newActions = [...selectedFunction.actions];
    newActions[index] = { ...newActions[index], [field]: value };
    onUpdateFunction({ actions: newActions });
  };

  const handleAddAction = () => {
    onUpdateFunction({
      actions: [...selectedFunction.actions, { t: ACT_SET_RELAY, ti: 0, v: 1, d: 60, tt: 0 }],
    });
  };

  const handleRemoveAction = (index) => {
    const newActions = selectedFunction.actions.filter((_, i) => i !== index);
    onUpdateFunction({ actions: newActions.length ? newActions : [{ t: ACT_SET_RELAY, ti: 0, v: 1, d: 60, tt: 0 }] });
  };

  return (
    <div className="flex flex-col gap-4">
      {/* Function metadata */}
      <DarkWrapperBlock className="!flex-col !items-stretch gap-3">
        <FormField label="Назва">
          <Input
            value={selectedFunction.name}
            onChange={(e) => onUpdateFunction({ name: e.target.value })}
            placeholder="Введіть назву"
            className="w-full"
          />
        </FormField>
        <div className="flex gap-4">
          <FormField label="Увімкнено">
            <SelectToggle
              value={selectedFunction.isEnabled}
              onChange={(e) => onUpdateFunction({ isEnabled: e.target.value === 'true' })}
            />
          </FormField>
          <FormField label="Пріоритет">
            <Input
              value={selectedFunction.priority ?? 0}
              isNumber
              onChange={(e) => onUpdateFunction({ priority: parseInt(e.target.value) || 0 })}
              className="w-20"
            />
          </FormField>
        </div>
      </DarkWrapperBlock>

      {/* Conditions */}
      <div className="flex flex-col gap-2">
        <h3 className="text-sm font-semibold text-gray-400 uppercase tracking-wider">Умови</h3>
        <ConditionNode
          node={selectedFunction.conditionTree}
          options={options}
          onChange={(newNode) => onUpdateFunction({ conditionTree: newNode ?? { ...EMPTY_CMP } })}
          isRoot={true}
        />
      </div>

      {/* Actions */}
      <div className="flex flex-col gap-2">
        <h3 className="text-sm font-semibold text-gray-400 uppercase tracking-wider">Дії</h3>
        <ActionList
          actions={selectedFunction.actions || []}
          options={options}
          onActionChange={handleActionChange}
          onAddAction={handleAddAction}
          onRemoveAction={handleRemoveAction}
        />
      </div>
    </div>
  );
};