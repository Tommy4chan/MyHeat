import { useState } from 'react';
import { MdVisibility, MdVisibilityOff } from 'react-icons/md';

const PasswordInput = ({ placeholder = '', value = '', onChange, className = '', maxLength, name = '' }) => {
  const [isVisible, setIsVisible] = useState(false);

  return (
    <div className={`relative ${className}`}>
      <input
        className='w-full pl-2 pr-9 py-1 text-white rounded focus:outline-none focus:ring transition duration-300 bg-gray-700 hover:bg-gray-800 focus:ring-gray-800'
        placeholder={placeholder}
        onChange={onChange}
        maxLength={maxLength}
        value={value ?? ''}
        type={isVisible ? 'text' : 'password'}
        name={name}
        autoComplete='off'
      />
      <button
        type='button'
        className='absolute inset-y-0 right-0 px-2 flex items-center text-gray-400 hover:text-white transition duration-300'
        onClick={() => setIsVisible(!isVisible)}
        title={isVisible ? 'Приховати' : 'Показати'}
      >
        {isVisible ? <MdVisibilityOff size={18} /> : <MdVisibility size={18} />}
      </button>
    </div>
  )
}

export default PasswordInput
