import DarkWrapperBlock from "../../../components/layout/DarkWrapperBlock"
import Button from "../../../components/ui/Button"
import FileUpload from "../../../components/ui/FileUpload"
import SettingsForm from "../../../components/layout/SettingsForm"
import FormField from "../../../components/ui/FormField"
import WrapperBlock from "../../../components/layout/WrapperBlock"
import useSettingStore from "../../../store/settingStore"
import { useEffect, useState } from "react"
import { showErrorToast, showSuccessToast, showWarningToast } from "../../../components/CustomToast"

const Device = () => {
  const { getAllDeviceSettings, allDeviceSettings, setAllDeviceSettings, restartDevice } = useSettingStore();
  const [shouldExport, setShouldExport] = useState(false);
  const [allSettingsData, setAllSettingsData] = useState({});

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
      showSuccessToast("Налаштування експортовано!");
    }
  }, [allDeviceSettings]);

  const handleExport = () => {
    getAllDeviceSettings();
    setShouldExport(true);
  }

  const handleFileChange = (event) => {
    const file = event.target.files[0];

    if (!file) return;

    const reader = new FileReader();
    reader.readAsText(file);

    reader.onload = (e) => {
      try {
        const parsedData = JSON.parse(e.target.result);
        setAllSettingsData(parsedData);
      } catch (err) {
        showErrorToast("Поганий формат файлу!");
        setAllSettingsData({});
      }
    };

    reader.onerror = () => {
      showErrorToast("Помилка при читанні файлу!");
    };
  };

  const handleImport = () => {
    if(Object.keys(allSettingsData).length === 0) {
      showWarningToast("Виберіть файл для імпорту!");
      return;
    }

    setAllDeviceSettings(allSettingsData);
  }

  return (
    <SettingsForm title='Пристрій'>
      <WrapperBlock>
        <h3 className='text-xl'>Імпорт налаштувань:</h3>
        <DarkWrapperBlock className="md:!flex-col">
          <FormField label='Файл налаштувань'>
            <FileUpload className='w-full' accept=".json" onChange={handleFileChange}/>
          </FormField>
        </DarkWrapperBlock>
        <Button buttonText='Імпортувати' color='indigo' className="w-full" onClick={handleImport}/>
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>Експорт налаштувань:</h3>
        <Button buttonText='Експортувати' color='indigo' className="w-full" onClick={handleExport} />
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>Керування</h3>
        <Button buttonText='Перезавантажити' color='yellow' className="w-full" onClick={restartDevice}/>
      </WrapperBlock>
    </SettingsForm>
  )
}

export default Device