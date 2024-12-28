import DarkWrapperBlock from "../../../components/layout/DarkWrapperBlock"
import Button from "../../../components/ui/Button"
import FileUpload from "../../../components/ui/FileUpload"
import FormColumn from "../../../components/layout/FormColumn"
import FormField from "../../../components/ui/FormField"
import WrapperBlock from "../../../components/layout/WrapperBlock"

const Device = () => {
  return (
    <FormColumn title='Пристрій'>
      <WrapperBlock>
        <h3 className='text-xl'>Імпорт налаштувань:</h3>
        <DarkWrapperBlock className="md:!flex-col">
          <FormField label='Файл налаштувань'>
            <FileUpload className='w-full' />
          </FormField>
        </DarkWrapperBlock>
        <Button buttonText='Імпортувати' color='indigo' className="w-full" />
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>Експорт налаштувань:</h3>
        <Button buttonText='Експортувати' color='indigo' className="w-full" />
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>Керування</h3>
        <Button buttonText='Перезавантажити' color='yellow' className="w-full" />
      </WrapperBlock>
    </FormColumn >
  )
}

export default Device