import DarkWrapperBlock from "@/components/layout/DarkWrapperBlock"
import Button from "@/components/ui/Button"
import FileUpload from "@/components/ui/FileUpload"
import SettingsForm from "@/components/layout/SettingsForm"
import FormField from "@/components/ui/FormField"
import WrapperBlock from "@/components/layout/WrapperBlock"
import { useDeviceSettings } from "../hooks/useDeviceSettings"

const Device = () => {
  const { handleExport, handleFileChange, handleImport, restartDevice } = useDeviceSettings()

  return (
    <SettingsForm title='Пристрій'>
      <WrapperBlock>
        <h3 className='text-xl'>Імпорт налаштувань:</h3>
        <DarkWrapperBlock className="md:!flex-col">
          <FormField label='Файл налаштувань'>
            <FileUpload className='w-full' accept=".json" onChange={handleFileChange} />
          </FormField>
        </DarkWrapperBlock>
        <Button buttonText='Імпортувати' color='indigo' className="w-full" onClick={handleImport} />
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>Експорт налаштувань:</h3>
        <Button buttonText='Експортувати' color='indigo' className="w-full" onClick={handleExport} />
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>Керування</h3>
        <Button buttonText='Перезавантажити' color='yellow' className="w-full" onClick={restartDevice} />
      </WrapperBlock>
    </SettingsForm>
  )
}

export default Device