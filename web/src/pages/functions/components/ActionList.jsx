import { MdClose } from 'react-icons/md';
import Select from '@/components/ui/Select';
import { ACT_SET_RELAY_TIMER } from '@/utils/functionEnums';
import { timerOptions, toTimerValue, fromTimerValue } from '../hooks/useFormOptions';
import { getActionHint } from '../utils/describe';
import { NumberInput, IconButton, AddButton, Hint } from './EditorControls';

// Firmware sends the duration only for timer actions, so a plain action switched to a timer has none
const DEFAULT_DURATION = 60;

const STATE_OPTIONS = [
  { value: 1, text: 'Увімкнути' },
  { value: 0, text: 'Вимкнути' },
];

export const ActionList = ({ actions, options, onActionChange, onAddAction, onRemoveAction }) => {
  const { getRelayOptions } = options;

  return (
    <div className='flex flex-col gap-2'>
      {actions.map((act, index) => (
        <div key={index} className='flex flex-col gap-1.5 bg-gray-900 border border-gray-700 border-l-2 border-l-yellow-600 rounded p-2'>
          <div className='flex flex-wrap items-center gap-2'>
            <Select value={act.v} options={STATE_OPTIONS} onChange={(e) => onActionChange(index, { v: +e.target.value })} />
            <Select value={act.ti} options={getRelayOptions(act.ti)} onChange={(e) => onActionChange(index, { ti: +e.target.value })} />
            <Select value={toTimerValue(act)} options={timerOptions} onChange={(e) => onActionChange(index, { d: act.d ?? DEFAULT_DURATION, ...fromTimerValue(e.target.value) })} />
            {act.t === ACT_SET_RELAY_TIMER && (
              <span className='flex items-center gap-1 text-sm text-gray-400'>
                на
                <NumberInput value={act.d ?? DEFAULT_DURATION} isInteger onChange={(d) => onActionChange(index, { d: Math.abs(d) })} className='w-20' />
                с
              </span>
            )}
            {actions.length > 1 && (
              <div className='ml-auto'>
                <IconButton icon={MdClose} isDanger title='Видалити дію' onClick={() => onRemoveAction(index)} />
              </div>
            )}
          </div>
          <Hint>{getActionHint(act)}</Hint>
        </div>
      ))}

      <div>
        <AddButton text='Дія' onClick={onAddAction} />
      </div>
    </div>
  );
};
