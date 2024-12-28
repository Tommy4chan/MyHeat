const DarkWrapperBlock = ({children, className=''}) => {
  return (
    <div className={`flex bg-gray-900 rounded shadow p-2 md:items-center flex-col md:flex-row gap-2 ${className}`}>{children}</div>
  )
}

export default DarkWrapperBlock;