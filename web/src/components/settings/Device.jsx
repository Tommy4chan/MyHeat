import DarkWrapperBlock from "../ui/DarkWrapperBlock"
import Button from "../ui/buttons/Button"
import FileUpload from "../ui/FileUpload"
import FormColumn from "../ui/FormColumn"
import FormField from "../ui/FormField"

const Device = () => {
  return (
    <FormColumn title='Пристрій'>
      <div className='flex flex-col gap-2 w-full'>
        <h3 className='text-xl'>Імпорт:</h3>
        <DarkWrapperBlock className="md:!flex-col">
          <FormField label='Файл налаштувань'>
            <FileUpload className='w-full' />
          </FormField>
        </DarkWrapperBlock>
        <Button buttonText='Імпортувати' color='indigo' className="w-full" />
      </div>
      <FormField label='Експорт'>
        <Button buttonText='Експортувати' color='indigo' className="w-full" />
      </FormField>
      <FormField label='Керування'>
        <Button buttonText='Перезавантажити' color='yellow' className="w-full" />
      </FormField>
    </FormColumn >
  )
}

export default Device