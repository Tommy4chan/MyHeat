import { useEffect, useState } from 'react';
import Input from '@/components/ui/Input';

// Keeps the typed text locally so partial input like "-" or "0." is not reset to 0 while typing
export const NumberInput = ({ value, onChange, isInteger = false, className = '' }) => {
  const [text, setText] = useState(String(value ?? 0));

  useEffect(() => {
    const parsed = isInteger ? parseInt(text) : parseFloat(text);
    if (parsed !== value) setText(String(value ?? 0));
    // Only react to external value changes, not to local typing
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [value]);

  const handleChange = (e) => {
    const next = e.target.value.replace(',', '.');
    if (!/^-?\d*\.?\d*$/.test(next)) return;
    setText(next);
    const parsed = isInteger ? parseInt(next) : parseFloat(next);
    onChange(isNaN(parsed) ? 0 : parsed);
  };

  return <Input value={text} onChange={handleChange} className={`text-right ${className}`} />;
};

export const IconButton = ({ icon: Icon, onClick, title, isActive = false, isDanger = false }) => (
  <button
    type='button'
    onClick={onClick}
    title={title}
    className={`p-1 rounded transition-colors ${isActive ? 'bg-gray-700 text-gray-200' : 'text-gray-500 hover:bg-gray-700'} ${isDanger ? 'hover:text-red-400' : 'hover:text-gray-200'}`}
  >
    <Icon size={18} />
  </button>
);

export const AddButton = ({ text, onClick }) => (
  <button
    type='button'
    onClick={onClick}
    className='text-xs text-gray-500 hover:text-gray-200 transition-colors px-2 py-1 bg-gray-800 hover:bg-gray-700 rounded border border-gray-700 border-dashed'
  >
    + {text}
  </button>
);

export const NotToggle = ({ isActive, onClick }) => (
  <button
    type='button'
    onClick={onClick}
    title={isActive ? 'Прибрати заперечення' : 'Заперечити (умова має НЕ виконуватись)'}
    className={`text-xs font-semibold px-1.5 py-0.5 rounded border transition-colors ${isActive ? 'bg-red-900/60 border-red-700 text-red-200' : 'border-gray-700 text-gray-600 hover:text-gray-300 hover:border-gray-500'}`}
  >
    НЕ
  </button>
);

export const Hint = ({ children }) => <p className='text-xs text-gray-500'>↳ {children}</p>;
