import Select from '@/components/ui/Select';
import Input from '@/components/ui/Input';
import Button from '@/components/ui/Button';
import FormField from '@/components/ui/FormField';
import {
  COND_AND, COND_OR, COND_NOT, COND_CMP,
  SRC_TEMP, SRC_RELAY, SRC_SMOKE,
  OP_EQ, OP_NEQ, OP_LT, OP_GT, OP_LTE, OP_GTE, OP_AVAILABLE, OP_UNAVAILABLE,
} from '@/utils/functionEnums';

// Left-border accent — the signature: encodes node type, used nowhere else
const NODE_ACCENT = {
  [COND_CMP]: 'border-l-blue-500',
  [COND_AND]: 'border-l-green-500',
  [COND_OR]:  'border-l-amber-500',
  [COND_NOT]: 'border-l-red-500',
};

const NODE_LABEL = {
  [COND_AND]: 'Всі умови нижче мають бути істинними',
  [COND_OR]:  'Хоча б одна умова нижче має бути істинною',
  [COND_NOT]: 'Умова нижче має бути хибною',
};

const EMPTY_CMP = {
  t: COND_CMP, src: SRC_TEMP, si: 0,
  d1: 0, d1sign: 1,                   // d1sign: 1=plus, -1=minus
  op: OP_LT,
  ivs: false, v: 25,
  src2: SRC_TEMP, si2: 0,
  d2: 0, d2sign: 1,
  hyst: 0.5,
  children: [],
};

// Which ops make sense per source type
const OPS_FOR_SOURCE = {
  [SRC_TEMP]:   [OP_LT, OP_LTE, OP_GT, OP_GTE, OP_EQ, OP_NEQ, OP_AVAILABLE, OP_UNAVAILABLE],
  [SRC_RELAY]:  [OP_EQ, OP_NEQ],
  [SRC_SMOKE]:  [OP_LT, OP_LTE, OP_GT, OP_GTE, OP_EQ, OP_NEQ, OP_AVAILABLE, OP_UNAVAILABLE],
};

const ALL_OPS = [
  { value: OP_LT,          text: '<  (менше)' },
  { value: OP_LTE,         text: '≤  (менше або рівне)' },
  { value: OP_GT,          text: '>  (більше)' },
  { value: OP_GTE,         text: '≥  (більше або рівне)' },
  { value: OP_EQ,          text: '=  (рівне)' },
  { value: OP_NEQ,         text: '≠  (не рівне)' },
  { value: OP_AVAILABLE,   text: 'Доступний (не помилка)' },
  { value: OP_UNAVAILABLE, text: 'Недоступний (помилка)' },
];

const SIGN_OPTIONS = [
  { value: 1,  text: '+' },
  { value: -1, text: '−' },
];

// Does this op need a right-hand side?
const hasRHS  = (op) => [OP_LT, OP_LTE, OP_GT, OP_GTE, OP_EQ, OP_NEQ].includes(op);
// Does this source benefit from delta correction and hysteresis?
const hasDelta = (src) => src === SRC_TEMP || src === SRC_SMOKE;
const hasHyst  = (src) => src === SRC_TEMP || src === SRC_SMOKE;

// Source selector + optional inline delta
const SourcePicker = ({ src, si, delta, deltaSign, onSrcChange, onSiChange, onDeltaChange, onDeltaSignChange, options }) => {
  const { sourceTypeOptions, getIndexOptions } = options;
  const showDelta = hasDelta(src);
  const indexOpts = getIndexOptions(src);

  return (
    <div className="flex flex-wrap items-center gap-2">
      <Select value={src} options={sourceTypeOptions} onChange={(e) => onSrcChange(+e.target.value)} />
      {indexOpts.length > 0 && (
        <Select value={si ?? 0} options={indexOpts} onChange={(e) => onSiChange(+e.target.value)} />
      )}
      {showDelta && (
        <>
          <Select value={deltaSign ?? 1} options={SIGN_OPTIONS} onChange={(e) => onDeltaSignChange(+e.target.value)} className="w-14" />
          <Input
            value={delta ?? 0}
            isNumber
            onChange={(e) => onDeltaChange(Math.abs(parseFloat(e.target.value) || 0))}
            className="w-20"
            placeholder="0"
          />
        </>
      )}
    </div>
  );
};

export const ConditionNode = ({ node, options, onChange, isRoot = false }) => {
  if (!node) return null;

  const { conditionTypeOptions, getIndexOptions } = options;

  const handleChange = (updates) => onChange({ ...node, ...updates });

  const handleTypeChange = (e) => {
    const t = +e.target.value;
    if (t === COND_CMP) onChange({ ...EMPTY_CMP });
    else onChange({ t, children: node.children || [] });
  };

  const handleChildChange = (idx, newChild) => {
    const children = [...(node.children || [])];
    if (newChild === null) children.splice(idx, 1);
    else children[idx] = newChild;
    onChange({ ...node, children });
  };

  const handleSrcChange = (newSrc) => {
    // Reset index and operator when source changes
    const allowed = OPS_FOR_SOURCE[newSrc] ?? [OP_LT];
    const op = allowed.includes(node.op) ? node.op : allowed[0];
    // Reset delta defaults
    const d1 = 0, d1sign = 1;
    // Relay src: auto-set op to EQ, value to 1 (ON) as sensible default
    const v = newSrc === SRC_RELAY ? 1 : (node.v ?? 25);
    handleChange({ src: newSrc, si: 0, op, d1, d1sign, v });
  };

  const showRHS   = hasRHS(node.op);
  const showHyst  = showRHS && hasHyst(node.src);
  const accentClass = NODE_ACCENT[node.t] ?? 'border-l-gray-600';

  // Build filtered op options for the current source
  const allowedOps = OPS_FOR_SOURCE[node.src] ?? ALL_OPS.map(o => o.value);
  const filteredOpOptions = ALL_OPS.filter(o => allowedOps.includes(o.value));

  // RHS relay-specific: if the relay state should only be ON/OFF
  const rhsIsRelayState = node.ivs && node.src2 === SRC_RELAY;

  return (
    <div className={`border-l-2 ${accentClass} pl-3 flex flex-col gap-2`}>
      {/* Header: type + delete */}
      <div className="flex items-center gap-2">
        <Select value={node.t} options={conditionTypeOptions} onChange={handleTypeChange} className="text-sm" />
        {!isRoot && (
          <button
            onClick={() => onChange(null)}
            className="ml-auto text-gray-500 hover:text-red-400 transition-colors text-xl leading-none"
            title="Видалити"
          >
            ×
          </button>
        )}
      </div>

      {/* Comparison body */}
      {node.t === COND_CMP && (
        <div className="flex flex-col gap-3 bg-gray-900 rounded p-3 border border-gray-700">

          {/* LHS */}
          <FormField label="Джерело">
            <SourcePicker
              src={node.src}
              si={node.si}
              delta={node.d1}
              deltaSign={node.d1sign ?? 1}
              onSrcChange={handleSrcChange}
              onSiChange={(v) => handleChange({ si: v })}
              onDeltaChange={(v) => handleChange({ d1: v })}
              onDeltaSignChange={(v) => handleChange({ d1sign: v })}
              options={options}
            />
          </FormField>

          {/* Operator — filtered to what makes sense for this source */}
          <FormField label="Умова">
            <Select value={node.op} options={filteredOpOptions} onChange={(e) => handleChange({ op: +e.target.value })} />
          </FormField>

          {/* RHS — only for standard comparison operators */}
          {showRHS && (
            <FormField label="Порівняти з">
              <div className="flex flex-col gap-2">
                <Select
                  value={node.ivs ? 'source' : 'const'}
                  options={[
                    { value: 'const',  text: 'Значенням' },
                    { value: 'source', text: 'Іншим джерелом' },
                  ]}
                  onChange={(e) => handleChange({ ivs: e.target.value === 'source' })}
                />

                {node.ivs ? (
                  <SourcePicker
                    src={node.src2 ?? SRC_TEMP}
                    si={node.si2}
                    delta={node.d2}
                    deltaSign={node.d2sign ?? 1}
                    onSrcChange={(v) => handleChange({ src2: v, si2: 0 })}
                    onSiChange={(v) => handleChange({ si2: v })}
                    onDeltaChange={(v) => handleChange({ d2: v })}
                    onDeltaSignChange={(v) => handleChange({ d2sign: v })}
                    options={options}
                  />
                ) : (
                  node.src === SRC_RELAY ? (
                    // Relay comparison: show ON/OFF picker instead of raw number
                    <Select
                      value={node.v ?? 1}
                      options={[
                        { value: 1, text: 'Увімкнено (ON)' },
                        { value: 0, text: 'Вимкнено (OFF)' },
                      ]}
                      onChange={(e) => handleChange({ v: +e.target.value })}
                    />
                  ) : (
                    <Input
                      value={node.v ?? 25}
                      isNumber
                      onChange={(e) => handleChange({ v: parseFloat(e.target.value) || 0 })}
                      className="w-32"
                      placeholder="25"
                    />
                  )
                )}
              </div>
            </FormField>
          )}

          {/* Hysteresis — only for temp/smoke, only when comparison is active */}
          {showHyst && (
            <FormField label="Гістерезис">
              <Input
                value={node.hyst ?? 0.5}
                isNumber
                onChange={(e) => handleChange({ hyst: Math.abs(parseFloat(e.target.value) || 0) })}
                className="w-28"
                placeholder="0.5"
              />
            </FormField>
          )}
        </div>
      )}

      {/* Logical body */}
      {node.t !== COND_CMP && (
        <div className="flex flex-col gap-3">
          {NODE_LABEL[node.t] && (
            <p className="text-xs text-gray-500">{NODE_LABEL[node.t]}</p>
          )}
          <div className="flex flex-col gap-3">
            {(node.children || []).map((child, idx) => (
              <ConditionNode
                key={idx}
                node={child}
                options={options}
                onChange={(newChild) => handleChildChange(idx, newChild)}
                isRoot={false}
              />
            ))}
          </div>
          {!(node.t === COND_NOT && (node.children || []).length >= 1) && (
            <button
              onClick={() => handleChildChange((node.children || []).length, { ...EMPTY_CMP })}
              className="self-start text-xs text-gray-500 hover:text-gray-200 transition-colors px-2 py-1 bg-gray-800 hover:bg-gray-700 rounded border border-gray-700 border-dashed"
            >
              + Додати умову
            </button>
          )}
        </div>
      )}
    </div>
  );
};
