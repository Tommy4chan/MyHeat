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

  const [otaProgress, setOtaProgress] = useState(0);
  const [selectedFirmware, setSelectedFirmware] = useState(null);
  const [selectedFs, setSelectedFs] = useState(null);

  const handleOtaFileChange = (type) => (event) => {
    const file = event.target.files[0];
    if (type === 'firmware') setSelectedFirmware(file || null);
    else if (type === 'fs') setSelectedFs(file || null);
  };

  const handleOtaUpdate = (type) => () => {
    const file = type === 'firmware' ? selectedFirmware : selectedFs;
    if (!file) {
      showWarningToast("Виберіть файл для оновлення!");
      return;
    }

    const formData = new FormData();
    formData.append("update", file, file.name);

    const xhr = new XMLHttpRequest();
    const url = import.meta.env.DEV ? `http://esp32-myheat-dev.local/update?type=${type}` : `/update?type=${type}`;
    xhr.open("POST", url, true);

    xhr.upload.onprogress = (e) => {
      if (e.lengthComputable) {
        setOtaProgress(Math.round((e.loaded / e.total) * 100));
      }
    };

    xhr.onload = () => {
      if (xhr.status === 200 && xhr.responseText === "OK") {
        showSuccessToast("Оновлення успішне. Перезавантаження...");
        setOtaProgress(100);
        setTimeout(() => location.reload(), 3000); // Wait for reboot then refresh
      } else {
        // Firmware replies "FAIL: <reason>", the reason helps tell a wrong file from a flash problem
        const reason = xhr.responseText?.startsWith("FAIL: ") ? xhr.responseText.slice(6) : "";
        showErrorToast(reason ? `Помилка оновлення пристрою: ${reason}` : "Помилка оновлення пристрою.");
        setOtaProgress(0);
      }
    };

    xhr.onerror = () => {
      showErrorToast("Помилка відправки оновлення.");
      setOtaProgress(0);
    };

    xhr.send(formData);
  };

  return {
    handleExport,
    handleFileChange,
    handleImport,
    restartDevice,
    handleOtaFileChange,
    handleOtaUpdate,
    otaProgress,
    selectedFirmware,
    selectedFs,
  };
};
