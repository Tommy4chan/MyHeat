const BasicButton = ({onClick, buttonText, color = 'gray', className = ''}) => {

    let styles = {
        gray: 'bg-gray-700 hover:bg-gray-800 focus:ring-gray-500',
        red: 'bg-red-700 hover:bg-red-800 focus:ring-red-500',
        yellow: 'bg-yellow-700 hover:bg-yellow-800 focus:ring-yellow-500',
        green: 'bg-green-700 hover:bg-green-800 focus:ring-green-500',
        blue: 'bg-blue-700 hover:bg-blue-800 focus:ring-blue-500',
        indigo: 'bg-indigo-700 hover:bg-indigo-800 focus:ring-indigo-500',
        purple: 'bg-purple-700 hover:bg-purple-800 focus:ring-purple-500',
        pink: 'bg-pink-700 hover:bg-pink-800 focus:ring-pink-500',
    }

    return (
        <button 
            className={`px-2 py-1 text-white rounded focus:outline-none focus:ring transition duration-300 ${styles[color]} ${className}`}
            onClick={onClick}
        >
            {buttonText}
        </button>
    )
}

export default BasicButton