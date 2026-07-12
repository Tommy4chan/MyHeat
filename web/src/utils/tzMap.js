import timezones from './timezones.json';

export const getPosixTZ = (ianaZone, offsetSeconds, hasDst) => {
  if (timezones[ianaZone]) {
    return timezones[ianaZone];
  }
  
  // Fallback: Construct a basic POSIX string if not found in map
  const hours = -Math.floor(offsetSeconds / 3600);
  const sign = hours >= 0 ? "+" : "";
  const baseTz = `UTC${sign}${hours}`;
  
  if (hasDst) {
    // If it has DST, append a generic DST rule (EU/US generic, rough approximation)
    return `${baseTz}DST`;
  }
  
  return baseTz;
};
