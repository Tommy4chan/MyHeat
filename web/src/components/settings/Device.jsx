import DarkWrapperBlock from "../ui/DarkWrapperBlock"
import Button from "../ui/buttons/Button"
import FileUpload from "../ui/FileUpload"
import FormColumn from "../ui/FormColumn"
import FormField from "../ui/FormField"
import WrapperBlock from "../ui/WrapperBlock"

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