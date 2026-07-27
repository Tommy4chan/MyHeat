import DarkWrapperBlock from "@/components/layout/DarkWrapperBlock"
import Button from "@/components/ui/Button"
import FileUpload from "@/components/ui/FileUpload"
import SettingsForm from "@/components/layout/SettingsForm"
import FormField from "@/components/ui/FormField"
import WrapperBlock from "@/components/layout/WrapperBlock"
import { useDeviceSettings } from "../hooks/useDeviceSettings"

const Device = () => {
  const { handleExport, handleFileChange, handleImport, restartDevice, handleOtaFileChange, handleOtaUpdate, otaProgress, selectedFirmware, selectedFs } = useDeviceSettings()

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
        <h3 className='text-xl'>Оновлення прошивки (OTA):</h3>
        <DarkWrapperBlock className="md:!flex-col">
          <div className="flex flex-col md:flex-row gap-4 w-full">
            <div className="flex-1 space-y-2">
              <FormField label='Прошивка (firmware.bin)'>
                <FileUpload className='w-full' accept=".bin" onChange={handleOtaFileChange('firmware')} disabled={otaProgress > 0 && otaProgress < 100} />
              </FormField>
              <Button buttonText='Оновити прошивку' color='indigo' className="w-full" onClick={handleOtaUpdate('firmware')} disabled={otaProgress > 0 && otaProgress < 100 || !selectedFirmware} />
            </div>
            
            <div className="flex-1 space-y-2">
              <FormField label='Файлова система (littlefs.bin)'>
                <FileUpload className='w-full' accept=".bin" onChange={handleOtaFileChange('fs')} disabled={otaProgress > 0 && otaProgress < 100} />
              </FormField>
              <Button buttonText='Оновити ФС' color='indigo' className="w-full" onClick={handleOtaUpdate('fs')} disabled={otaProgress > 0 && otaProgress < 100 || !selectedFs} />
            </div>
          </div>
          
          {otaProgress > 0 && (
            <div className="w-full mt-2">
              <div className="flex justify-between mb-1">
                <span className={`text-sm font-medium ${otaProgress === 100 ? "text-green-500" : "text-gray-300"}`}>
                  {otaProgress === 100 ? "Завершено! Перезавантаження..." : "Завантаження..."}
                </span>
                <span className="text-sm font-medium text-gray-300">{otaProgress}%</span>
              </div>
              <div className="w-full bg-gray-700 rounded-full h-2.5">
                <div 
                  className={`h-2.5 rounded-full transition-all duration-300 ${otaProgress === 100 ? "bg-green-500" : "bg-indigo-500"}`} 
                  style={{ width: `${otaProgress}%` }}
                ></div>
              </div>
            </div>
          )}
        </DarkWrapperBlock>
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