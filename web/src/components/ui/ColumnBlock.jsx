const ColumnBlock = ({ children, className = '' }) => {
  return (
    <div className={`flex flex-col bg-gray-800 border border-gray-700 rounded-lg shadow-md p-4 gap-4 ${className}`}>{children}</div>
  )
}

export default ColumnBlock