import { useEffect, useState } from "react";
import useSettingStore from "@/store/settingStore";
import { showErrorToast, showSuccessToast, showWarningToast } from "@/components/CustomToast";

export const useDeviceSettings = () => {
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

  return {
    handleExport,
    handleFileChange,
    handleImport,
    restartDevice,
  };
};
