const WrapperBlock = ({ children }) => {
  return (
    <div className='flex flex-col gap-2 w-full'>
      {children}
    </div>
  )
}

export default WrapperBlock