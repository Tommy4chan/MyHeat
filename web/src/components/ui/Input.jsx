const Input = ({ placeholder = '', value = '', onChange, className='', maxLength, isNumber = false }) => {
  return (
    <input
      className={`px-2 py-1 text-white rounded focus:outline-none focus:ring transition duration-300 bg-gray-700 hover:bg-gray-800 focus:ring-gray-800 ${className} ${isNumber && '[appearance:textfield] [&::-webkit-outer-spin-button]:appearance-none [&::-webkit-inner-spin-button]:appearance-none'}`}
      placeholder={placeholder} 
      onChange={onChange}
      maxLength={maxLength}
      defaultValue={value || ''}
      type={isNumber ? 'number' : 'text'}
    />
  )
}

export default Input