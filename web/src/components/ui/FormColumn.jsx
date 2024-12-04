import ColumnBlock from './ColumnBlock';

const FormColumn = ({ title, children }) => {
  return (
    <ColumnBlock className='break-inside'>
      <h2 className='font-semibold text-2xl'>{title}</h2>
      {children}
    </ColumnBlock>
  );
};

export default FormColumn;