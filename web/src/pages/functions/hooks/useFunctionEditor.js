import { useState, useEffect } from "react";
import useFunctionStore from "@/store/functionStore";
import { EMPTY_ACTION, toEditorTree, fromEditorTree } from "../utils/conditionTree";

const EMPTY_FUNCTION = {
  name: "Нова функція",
  isEnabled: false,
  isActive: false,
  priority: 0,
  conditionTree: toEditorTree([]),
  actions: [{ ...EMPTY_ACTION }],
};

export const useFunctionEditor = () => {
  const [functionList, setFunctionList] = useState([]);
  const [selectedIndex, setSelectedIndex] = useState(0);
  const [isEdited, setIsEdited] = useState(false);
  const { functions, setFunctionsSettings } = useFunctionStore();

  useEffect(() => {
    // Don't overwrite user's unsaved edits with background WS updates
    if (!isEdited) {
      const initialFunctions =
        functions.length > 0
          ? functions.map((f) => ({
              ...f,
              conditionTree: toEditorTree(f.conditions),
            }))
          : [JSON.parse(JSON.stringify(EMPTY_FUNCTION))];
      setFunctionList(initialFunctions);
    }
  }, [functions, isEdited]);

  const handleFunctionSelect = (e) => {
    const newIndex = +e.target.value;
    if (newIndex === functionList.length) {
      setFunctionList((prev) => [
        ...prev,
        JSON.parse(JSON.stringify(EMPTY_FUNCTION)),
      ]);
      setIsEdited(true);
    }
    setSelectedIndex(newIndex);
  };

  const updateFunction = (updates) => {
    setIsEdited(true);
    setFunctionList((prev) =>
      prev.map((func, index) =>
        index === selectedIndex ? { ...func, ...updates } : func
      )
    );
  };

  const serialize = (funcList) =>
    funcList.map((f) => {
      const out = { ...f, conditions: fromEditorTree(f.conditionTree) };
      delete out.conditionTree;
      return out;
    });

  const handleSave = () => {
    setFunctionsSettings(serialize(functionList));
    setIsEdited(false);
  };

  const handleDelete = () => {
    const newFunctions =
      functionList.length === 1
        ? [JSON.parse(JSON.stringify(EMPTY_FUNCTION))]
        : functionList.filter((_, i) => i !== selectedIndex);
    setFunctionList(newFunctions);
    setSelectedIndex((prev) => Math.max(0, prev === 0 ? 0 : prev - 1));
    setFunctionsSettings(serialize(newFunctions));
    setIsEdited(false);
  };

  return {
    functionList,
    selectedIndex,
    selectedFunction: functionList[selectedIndex],
    isEdited,
    updateFunction,
    handleFunctionSelect,
    handleSave,
    handleDelete,
  };
};
