import DarkWrapperBlock from "../../../components/layout/DarkWrapperBlock"
import Button from "../../../components/ui/Button"
import FileUpload from "../../../components/ui/FileUpload"
import SettingsForm from "../../../components/layout/SettingsForm"
import FormField from "../../../components/ui/FormField"
import WrapperBlock from "../../../components/layout/WrapperBlock"
import useSettingStore from "../../../store/settingStore"
import { useEffect, useState } from "react"

const Device = () => {
  const { getAllDeviceSettings, allDeviceSettings } = useSettingStore();
  const [shouldExport, setShouldExport] = useState(false);

  useEffect(() => {
    if (shouldExport) {
      const jsonString = JSON.stringify(allDeviceSettings, null, 2);
      const blob = new Blob([jsonString], { type: "application/json" });
      const url = URL.createObjectURL(blob);

      const a = document.createElement("a");
      a.href = url;
      a.download = "data.json";
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
      URL.revokeObjectURL(url);
      setShouldExport(false);
    }
  }, [allDeviceSettings]);

  const handleExport = () => {
    getAllDeviceSettings();
    setShouldExport(true);
  }

  return (
    <SettingsForm title='Пристрій'>
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
        <Button buttonText='Експортувати' color='indigo' className="w-full" onClick={handleExport} />
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>Керування</h3>
        <Button buttonText='Перезавантажити' color='yellow' className="w-full" />
      </WrapperBlock>
    </SettingsForm>
  )
}

export default Device