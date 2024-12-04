const FormField = ({ label, children }) => {
  return (
    <div className='w-full'>
      <label className='block mb-1 text-sm text-white'>{label}:</label>
      {children}
    </div>
  );
};

export default FormField;