import { useEffect, useState } from "react";
import { DateTime } from "luxon";
import useSettingStore from "@/store/settingStore";

export const useNTPSettings = () => {
  const [ntpServer, setNtpServer] = useState('');
  const [ntpData, setNtpData] = useState({offset: 0, dst: false});
  const [timeZone, setTimeZone] = useState(Intl.DateTimeFormat().resolvedOptions().timeZone);

  const { setNtpSettings, getNtpSettings, ntpSettings } = useSettingStore();

  const timeZoneOptions = Intl.supportedValuesOf('timeZone').map(tz => ({
    value: tz,
    text: tz.replace('_', ' ')
  }));

  useEffect(() => {
    getNtpSettings();
  }, []);

  useEffect(() => {
    setNtpServer(ntpSettings.ntpServer);
    setTimeZone(ntpSettings.ntpIANA);
  }, [ntpSettings]);

  const handleChangeTimeZone = (e) => {
    const newTimeZone = e.target.value;
    setTimeZone(newTimeZone);

    const now = DateTime.now().setZone(newTimeZone);
    let result = { offset: 0, dst: false };

    if (!now.isValid) {
      console.error("Invalid timezone:", newTimeZone);
      return;
    }

    result.offset = now.offset * 60;

    const janOffset = DateTime.local(now.year, 1, 1).setZone(
      newTimeZone
    ).offset;
    const julOffset = DateTime.local(now.year, 7, 1).setZone(
      newTimeZone
    ).offset;

    result.dst = janOffset !== julOffset;
    
    setNtpData(result);
  };

  const handleSaveNTPSettings = () => {
    setNtpSettings(ntpServer, timeZone, ntpData.offset, ntpData.dst);
  }

  return {
    ntpServer,
    setNtpServer,
    timeZone,
    timeZoneOptions,
    handleChangeTimeZone,
    handleSaveNTPSettings
  };
};
