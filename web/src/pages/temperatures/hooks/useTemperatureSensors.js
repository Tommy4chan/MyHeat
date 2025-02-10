import { useState, useEffect } from "react";
import useTemperatureStore from "@/store/temperatureStore";

export const useTemperatureSensors = () => {
  const {
    temperatureSettings,
    getDiscoveredTemperatureSensors,
    discoveredTemperatureSensors,
    setTemperatureSensor,
  } = useTemperatureStore();

  const [temperatureSensors, setTemperatureSensors] = useState([]);

  const temperatureOptions = Array.from(
    { length: temperatureSettings.temperatureCount },
    (_, i) => ({
      value: i,
      text: `T${i}`,
    })
  );

  useEffect(() => {
    setTemperatureSensors(discoveredTemperatureSensors);
  }, [discoveredTemperatureSensors]);

  const handleSensorTempIndexChange = (sensorAddressIndex) => (e) => {
    setTemperatureSensors((prev) =>
      prev.map((sensor) =>
        sensor.id === sensorAddressIndex
          ? { ...sensor, tempIndex: e.target.value }
          : sensor
      )
    );
  };

  const handleSensorSave = (index) => () => {
    const sensor = temperatureSensors[index];
    setTemperatureSensor(sensor.tempIndex, sensor.id);
  };

  return {
    temperatureSensors,
    temperatureOptions,
    handleSensorTempIndexChange,
    handleSensorSave,
    getDiscoveredTemperatureSensors,
  };
};
