import { useState, useEffect } from "react";
import useFunctionStore from "@/store/functionStore";

const EMPTY_FUNCTION = {
  sign: 0,
  temperatureIndex: [0, 0],
  deltaValueSign: [0, 0],
  deltaValue: [0, 0],
  relayIndex: 0,
  isEnabled: false,
  isActive: false,
};

export const useFunctionEditor = () => {
  const [functionList, setFunctionList] = useState([]);
  const [selectedIndex, setSelectedIndex] = useState(0);
  const { functions, setFunctionsSettings, tnIndex } = useFunctionStore();

  useEffect(() => {
    const initialFunctions =
      functions.length > 0 ? functions : [EMPTY_FUNCTION];
    setFunctionList(initialFunctions);
  }, []);

  const handleFunctionSelect = (e) => {
    const newIndex = +e.target.value;
    if (newIndex === functionList.length) {
      setFunctionList((prev) => [...prev, EMPTY_FUNCTION]);
    }
    setSelectedIndex(newIndex);
  };

  const updateFunction = (updates) => {
    setFunctionList((prev) =>
      prev.map((func, index) =>
        index === selectedIndex ? { ...func, ...updates } : func
      )
    );
  };

  const handleTemperatureChange = (temperatureIndex) => (e) => {
    const temperatureIndexValue = parseInt(e.target.value) || 0;

    updateFunction({
      temperatureIndex: functionList[selectedIndex].temperatureIndex.map(
        (val, index) =>
          index === temperatureIndex ? temperatureIndexValue : val
      ),
    });
  };

  const handleDeltaValueChange = (deltaIndex) => (e) => {
    const value = parseInt(e.target.value) || 0;

    updateFunction({
      deltaValue: functionList[selectedIndex].deltaValue.map((val, index) =>
        index === deltaIndex ? Math.abs(value) : val
      ),
    });
  };

  const handleDeltaSignChange = (signIndex) => (e) => {
    updateFunction({
      deltaValueSign: functionList[selectedIndex].deltaValueSign.map(
        (val, index) => (index === signIndex ? +e.target.value : val)
      ),
    });
  };

  const handleSignChange = (e) => {
    updateFunction({ sign: e.target.value });
  };

  const handleRelayChange = (e) => {
    updateFunction({ relayIndex: e.target.value });
  };

  const handleSave = () => {
    setFunctionsSettings(functionList);
  };

  const handleDelete = () => {
    let newFunctions =
      functionList.length === 1
        ? [EMPTY_FUNCTION]
        : functionList.filter((_, index) => index !== selectedIndex);

    setFunctionList(newFunctions);
    setSelectedIndex((prev) => (prev === 0 ? 0 : prev - 1));
    setFunctionsSettings(newFunctions);
  };

  return {
    functionList,
    selectedIndex,
    selectedFunction: functionList[selectedIndex],
    handleFunctionSelect,
    handleTemperatureChange,
    handleDeltaValueChange,
    handleDeltaSignChange,
    handleSignChange,
    handleRelayChange,
    handleSave,
    handleDelete,
  };
};
