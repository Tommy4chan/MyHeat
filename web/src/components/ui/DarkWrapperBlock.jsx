const DarkWrapperBlock = ({children, className=''}) => {
  return (
    <div className={`flex bg-gray-900 rounded shadow p-2 items-center ${className}`}>{children}</div>
  )
}

export default DarkWrapperBlock;