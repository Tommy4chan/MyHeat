import Select from '@/components/ui/Select';
import Input from '@/components/ui/Input';
import Button from '@/components/ui/Button';
import FormField from '@/components/ui/FormField';
import { ACT_SET_RELAY_TIMER } from '@/utils/functionEnums';

export const ActionList = ({ actions, options, onActionChange, onAddAction, onRemoveAction }) => {
  const { actionTypeOptions, relayOptions, relayStateOptions } = options;

  return (
    <div className="flex flex-col gap-3">
      {actions.map((act, index) => (
        <div key={index} className="border-l-2 border-l-yellow-600 pl-3 flex flex-col gap-2">
          <div className="flex items-center gap-2">
            <span className="text-sm text-gray-400">Дія {index + 1}</span>
            {actions.length > 1 && (
              <button
                onClick={() => onRemoveAction(index)}
                className="ml-auto text-gray-500 hover:text-red-400 transition-colors text-lg leading-none"
                title="Видалити"
              >
                ×
              </button>
            )}
          </div>

          <div className="flex flex-col gap-2 bg-gray-900 rounded p-3 border border-gray-700">
            <FormField label="Тип">
              <Select
                value={act.t}
                options={actionTypeOptions}
                onChange={(e) => onActionChange(index, 't', +e.target.value)}
              />
            </FormField>

            <FormField label="Реле">
              <Select
                value={act.ti}
                options={relayOptions}
                onChange={(e) => onActionChange(index, 'ti', +e.target.value)}
              />
            </FormField>

            <FormField label="Стан">
              <Select
                value={act.v}
                options={relayStateOptions}
                onChange={(e) => onActionChange(index, 'v', +e.target.value)}
              />
            </FormField>

            {act.t === ACT_SET_RELAY_TIMER && (
              <>
                <FormField label="Режим">
                  <Select
                    value={act.tt ?? 0}
                    options={options.timerTypeOptions}
                    onChange={(e) => onActionChange(index, 'tt', +e.target.value)}
                  />
                </FormField>
                <FormField label="Тривалість (сек)">
                  <Input
                    value={act.d ?? 60}
                    isNumber
                    onChange={(e) => onActionChange(index, 'd', parseInt(e.target.value) || 0)}
                    className="w-32"
                  />
                </FormField>
              </>
            )}
          </div>
        </div>
      ))}

      <button
        onClick={onAddAction}
        className="self-start text-xs text-gray-500 hover:text-gray-200 transition-colors px-2 py-1 bg-gray-800 hover:bg-gray-700 rounded border border-gray-700 border-dashed"
      >
        + Додати дію
      </button>
    </div>
  );
};
