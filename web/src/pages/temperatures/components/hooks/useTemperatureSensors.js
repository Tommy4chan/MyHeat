import { useState, useEffect, useCallback, useMemo } from 'react';
import useTemperatureStore from '../../../../store/temperatureStore';

export const useTemperatureSensors = () => {
  const {
    temperatureSettings,
    getDiscoveredTemperatureSensors,
    discoveredTemperatureSensors,
    setTemperatureSensor,
  } = useTemperatureStore();

  const [temperatureSensors, setTemperatureSensors] = useState([]);

  const temperatureOptions = useMemo(
    () =>
      Array.from({ length: temperatureSettings.temperatureCount }, (_, i) => ({
        value: i,
        text: `T${i}`,
      })),
    [temperatureSettings.temperatureCount]
  );

  useEffect(() => {
    setTemperatureSensors(discoveredTemperatureSensors);
  }, [discoveredTemperatureSensors]);

  const handleSensorTempIndexChange = useCallback(
    (sensorAddressIndex) => (e) => {
      setTemperatureSensors((prev) =>
        prev.map((sensor) =>
          sensor.id === sensorAddressIndex
            ? { ...sensor, tempIndex: e.target.value }
            : sensor
        )
      );
    },
    []
  );

  const handleSensorSave = useCallback(
    (index) => () => {
      const sensor = temperatureSensors[index];
      setTemperatureSensors((prev) =>
        prev.filter((_, i) => i !== index)
      );
      setTemperatureSensor(sensor.tempIndex, sensor.id);
    },
    [temperatureSensors, setTemperatureSensor]
  );

  return {
    temperatureSensors,
    temperatureOptions,
    handleSensorTempIndexChange,
    handleSensorSave,
    getDiscoveredTemperatureSensors,
  };
};