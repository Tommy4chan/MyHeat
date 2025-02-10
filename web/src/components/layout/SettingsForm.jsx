import ColumnBlock from './ColumnBlock';

const SettingsForm = ({ title, children }) => {
  return (
    //<ColumnBlock className='break-inside'>
    <ColumnBlock>
      <h2 className='font-semibold text-2xl'>{title}</h2>
      {children}
    </ColumnBlock>
  );
};

export default SettingsForm;