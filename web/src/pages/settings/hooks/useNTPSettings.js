import { useState } from "react";
import { DateTime } from "luxon";

export const useNTPSettings = () => {
  const [ntpServer, setNtpServer] = useState('');
  const [timeZone, setTimeZone] = useState(Intl.DateTimeFormat().resolvedOptions().timeZone);

  const timeZoneOptions = Intl.supportedValuesOf('timeZone').map(tz => ({
    value: tz,
    text: tz.replace('_', ' ')
  }));

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

    console.log(result);
  };

  const handleSaveNTPSettings = () => {
    console.log('Save NTP settings');
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
