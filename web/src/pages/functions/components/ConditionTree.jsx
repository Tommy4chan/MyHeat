import { useState } from 'react';
import { MdTune, MdClose, MdSwapHoriz } from 'react-icons/md';
import Select from '@/components/ui/Select';
import {
  COND_AND, COND_OR, COND_NOT,
  SRC_TEMP,
  OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LTE, OP_GTE, OP_AVAILABLE, OP_UNAVAILABLE,
} from '@/utils/functionEnums';
import { newCondition, newGroup, isGroup } from '../utils/conditionTree';
import { OP_SYMBOLS, isBooleanSource, getBooleanLabels, getUnit, getSourceName, getHysteresisHint } from '../utils/describe';
import { toSourceValue, fromSourceValue } from '../hooks/useFormOptions';
import { NumberInput, IconButton, AddButton, NotToggle, Hint } from './EditorControls';

// Left-border accent encodes the node type
const GROUP_ACCENT = {
  [COND_AND]: 'border-l-green-500',
  [COND_OR]:  'border-l-amber-500',
};

const GROUP_TYPE_OPTIONS = [
  { value: COND_AND, text: 'всі умови' },
  { value: COND_OR,  text: 'хоча б одна умова' },
];

const NUMERIC_OP_OPTIONS = [OP_LT, OP_LTE, OP_GT, OP_GTE, OP_EQ, OP_NEQ, OP_AVAILABLE, OP_UNAVAILABLE]
  .map((op) => ({ value: op, text: OP_SYMBOLS[op] }));

const BOOLEAN_OP_OPTIONS = [
  { value: OP_EQ,  text: 'є' },
  { value: OP_NEQ, text: 'не є' },
];

// Root (depth 0) and one nested level may hold groups, so the tree is at most 3 levels deep
const MAX_GROUP_DEPTH = 2;

const signed = (magnitude, sign) => Math.abs(magnitude ?? 0) * (sign ?? 1);
const isAvailabilityOp = (op) => op === OP_AVAILABLE || op === OP_UNAVAILABLE;

const ConditionRow = ({ node, options, onChange, onRemove, notToggle }) => {
  const { getSourceOptions, getLiveValue } = options;
  const si = node.si ?? 0;
  const delta1 = signed(node.d1, node.d1sign);
  const delta2 = signed(node.d2, node.d2sign);
  const [isExtrasOpen, setIsExtrasOpen] = useState(delta1 !== 0 || (node.ivs && delta2 !== 0));

  const update = (updates) => onChange({ ...node, ...updates });

  const isBoolean = isBooleanSource(node.src, si) && !node.ivs;
  const hasRhs = !isBoolean && !isAvailabilityOp(node.op);
  const liveValue = getLiveValue(node.src, si);
  const liveValue2 = node.ivs ? getLiveValue(node.src2 ?? SRC_TEMP, node.si2 ?? 0) : null;
  const unit = getUnit(node.src);
  const hint = hasRhs ? getHysteresisHint(node) : null;

  const handleSourceChange = (e) => {
    const [src, nextSi] = fromSourceValue(e.target.value);
    const wasBoolean = isBooleanSource(node.src, si);
    if (isBooleanSource(src, nextSi)) {
      update({ src, si: nextSi, ivs: false, d1: 0, d1sign: 1, op: wasBoolean ? node.op : OP_EQ, v: wasBoolean ? node.v : 1 });
    } else if (wasBoolean) {
      update({ src, si: nextSi, op: OP_LT, v: src === SRC_TEMP ? 25 : 300, hyst: src === SRC_TEMP ? 0.5 : 20 });
    } else {
      update({ src, si: nextSi });
    }
  };

  const handleSource2Change = (e) => {
    const [src2, si2] = fromSourceValue(e.target.value);
    update({ src2, si2 });
  };

  return (
    <div className='flex flex-col gap-1.5 bg-gray-900 border border-gray-700 border-l-2 border-l-blue-500 rounded p-2'>
      <div className='flex flex-wrap items-center gap-2'>
        {notToggle}
        <Select value={toSourceValue(node.src, si)} options={getSourceOptions(node.src, si)} onChange={handleSourceChange} />
        {liveValue && <span className='text-xs text-gray-500 whitespace-nowrap'>зараз {liveValue}</span>}

        {isBoolean ? (
          <>
            <Select value={node.op === OP_NEQ ? OP_NEQ : OP_EQ} options={BOOLEAN_OP_OPTIONS} onChange={(e) => update({ op: +e.target.value })} />
            <Select
              value={(node.v ?? 1) === 1 ? 1 : 0}
              options={Object.entries(getBooleanLabels(node.src)).reverse().map(([value, text]) => ({ value: +value, text }))}
              onChange={(e) => update({ v: +e.target.value })}
            />
          </>
        ) : (
          <>
            <Select value={node.op} options={NUMERIC_OP_OPTIONS} onChange={(e) => update({ op: +e.target.value })} />
            {hasRhs && (node.ivs ? (
              <>
                <Select
                  value={toSourceValue(node.src2 ?? SRC_TEMP, node.si2)}
                  options={getSourceOptions(node.src2 ?? SRC_TEMP, node.si2 ?? 0)}
                  onChange={handleSource2Change}
                />
                {liveValue2 && <span className='text-xs text-gray-500 whitespace-nowrap'>зараз {liveValue2}</span>}
              </>
            ) : (
              <span className='flex items-center gap-1'>
                <NumberInput value={node.v ?? 0} onChange={(v) => update({ v })} className='w-20' />
                {unit && <span className='text-sm text-gray-400'>{unit}</span>}
              </span>
            ))}
            {hasRhs && (
              <IconButton
                icon={MdSwapHoriz}
                isActive={node.ivs}
                title={node.ivs ? 'Порівнювати з числом' : 'Порівнювати з іншим датчиком'}
                onClick={() => update({ ivs: !node.ivs })}
              />
            )}
          </>
        )}

        <div className='ml-auto flex items-center gap-1'>
          {hasRhs && <IconButton icon={MdTune} isActive={isExtrasOpen} title='Поправка і гістерезис' onClick={() => setIsExtrasOpen(!isExtrasOpen)} />}
          {onRemove && <IconButton icon={MdClose} isDanger title='Видалити умову' onClick={onRemove} />}
        </div>
      </div>

      {hasRhs && isExtrasOpen && (
        <div className='flex flex-wrap items-center gap-x-4 gap-y-2 text-sm text-gray-400'>
          <label className='flex items-center gap-2'>
            Поправка {getSourceName(node.src, si)}
            <NumberInput value={delta1} onChange={(d) => update({ d1: Math.abs(d), d1sign: d < 0 ? -1 : 1 })} className='w-20' />
          </label>
          {node.ivs && (
            <label className='flex items-center gap-2'>
              Поправка {getSourceName(node.src2 ?? SRC_TEMP, node.si2 ?? 0)}
              <NumberInput value={delta2} onChange={(d) => update({ d2: Math.abs(d), d2sign: d < 0 ? -1 : 1 })} className='w-20' />
            </label>
          )}
          <label className='flex items-center gap-2'>
            Гістерезис
            <NumberInput value={node.hyst ?? 0} onChange={(h) => update({ hyst: Math.abs(h) })} className='w-20' />
          </label>
        </div>
      )}

      {hint && <Hint>{hint}</Hint>}
    </div>
  );
};

// Renders a NOT node as its child with the НЕ toggle switched on
const ConditionItem = ({ node, options, depth, onChange, onRemove }) => {
  const isNegated = node.t === COND_NOT;
  const inner = isNegated ? node.children[0] : node;

  const handleInnerChange = (newInner) => onChange(isNegated ? { ...node, children: [newInner] } : newInner);
  const notToggle = <NotToggle isActive={isNegated} onClick={() => onChange(isNegated ? inner : { t: COND_NOT, children: [node] })} />;

  return isGroup(inner) ? (
    <ConditionGroup node={inner} options={options} depth={depth} onChange={handleInnerChange} onRemove={onRemove} notToggle={notToggle} />
  ) : (
    <ConditionRow node={inner} options={options} onChange={handleInnerChange} onRemove={onRemove} notToggle={notToggle} />
  );
};

export const ConditionGroup = ({ node, options, onChange, depth = 0, onRemove, notToggle }) => {
  const children = node.children || [];
  const isRoot = depth === 0;

  const handleChildChange = (index, child) => {
    const next = [...children];
    next[index] = child;
    onChange({ ...node, children: next });
  };

  // Removing the last child of a nested group removes the group, the root always keeps one condition
  const handleChildRemove = (index) => {
    const next = children.filter((_, i) => i !== index);
    if (next.length === 0 && onRemove) onRemove();
    else onChange({ ...node, children: next.length ? next : [newCondition()] });
  };

  const canRemoveChild = children.length > 1 || !isRoot;

  return (
    <div className={`flex flex-col gap-2 ${isRoot ? '' : `border-l-2 ${GROUP_ACCENT[node.t]} pl-3`}`}>
      {(children.length > 1 || !isRoot) && (
        <div className='flex flex-wrap items-center gap-2 text-sm text-gray-400'>
          {notToggle}
          <span>мають виконуватись</span>
          <Select value={node.t} options={GROUP_TYPE_OPTIONS} onChange={(e) => onChange({ ...node, t: +e.target.value })} />
          {onRemove && (
            <div className='ml-auto'>
              <IconButton icon={MdClose} isDanger title='Видалити групу' onClick={onRemove} />
            </div>
          )}
        </div>
      )}

      {children.map((child, index) => (
        <ConditionItem
          key={index}
          node={child}
          options={options}
          depth={depth + 1}
          onChange={(newChild) => handleChildChange(index, newChild)}
          onRemove={canRemoveChild ? () => handleChildRemove(index) : null}
        />
      ))}

      <div className='flex flex-wrap gap-2'>
        <AddButton text='Умова' onClick={() => onChange({ ...node, children: [...children, newCondition()] })} />
        {depth < MAX_GROUP_DEPTH && (
          <AddButton text='Група' onClick={() => onChange({ ...node, children: [...children, newGroup()] })} />
        )}
      </div>
    </div>
  );
};
