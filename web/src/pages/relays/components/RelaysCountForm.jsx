import DarkWrapperBlock from '../../../components/layout/DarkWrapperBlock';
import FormField from '../../../components/ui/FormField';
import Input from '../../../components/ui/Input';
import SaveButton from '../../../components/ui/SaveButton';

export const RelayCountForm = ({ count, onChange, onSave }) => {
  return (
    <div className="flex flex-col gap-2">
      <DarkWrapperBlock className="md:!flex-col">
        <FormField label='Кількість реле'>
          <Input
            value={count}
            onChange={onChange}
            isNumber={true}
            maxLength={2}
            className='w-full'
          />
        </FormField>
      </DarkWrapperBlock>
      <SaveButton onClick={onSave} />
    </div>
  );
};