const Select = ({ value, options, color = 'gray', onChange, className, name = '' }) => {
    const styles = {
        gray: 'bg-gray-700 hover:bg-gray-800 focus:ring-gray-800',
        'light-gray': 'bg-gray-600 hover:bg-gray-700 focus:ring-gray-700',
    }

    return (
        <select
            value={value ?? 0}
            className={`px-2 py-1 text-white rounded focus:outline-none focus:ring transition duration-300 ${styles[color]} ${className}`}
            onChange={onChange}
            name={name}
        >
            {options.map((option, index) => (
                <option key={index} value={option.value} disabled={option?.isDisabled}>{option.text}</option>
            ))}
        </select>
    )
}

export default Select