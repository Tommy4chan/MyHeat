import { useState, useEffect } from "react";
import useFunctionStore from "@/store/functionStore";
import { COND_CMP, SRC_TEMP, OP_LT, ACT_SET_RELAY } from "@/utils/functionEnums";

// Apply sign to magnitude before sending to C++
const applySign = (mag, sign) => Math.abs(mag ?? 0) * (sign ?? 1);

// Default empty node — matches what C++ expects when ivs=false
// When ivs=false, the constant value is serialized as "v", not "d2"
const EMPTY_CMP_NODE = {
  t: COND_CMP,
  src: SRC_TEMP,
  si: 0,
  d1: 0,
  d1sign: 1,
  op: OP_LT,
  ivs: false,
  v: 25,       // constant value (used when ivs=false)
  src2: SRC_TEMP,
  si2: 0,
  d2: 0,       // RHS delta magnitude (used when ivs=true)
  d2sign: 1,
  hyst: 0.5,
  children: [],
};

const EMPTY_ACTION = { t: ACT_SET_RELAY, ti: 0, v: 1, d: 60, tt: 0 };

const EMPTY_FUNCTION = {
  name: "Нова функція",
  isEnabled: false,
  isActive: false,
  priority: 0,
  conditionTree: { ...EMPTY_CMP_NODE },
  actions: [{ ...EMPTY_ACTION }],
};

// Convert flat conditions array from server → nested tree for UI
const unflattenConditions = (flatArray) => {
  if (!flatArray || flatArray.length === 0) return { ...EMPTY_CMP_NODE };

  const buildNode = (index) => {
    if (index < 0 || index >= flatArray.length) return { ...EMPTY_CMP_NODE };
    const raw = flatArray[index];
    const node = { ...raw, children: [] };

    if (raw.t !== COND_CMP) {
      // Logical node: build children from childStart + childCount
      const cs = raw.cs ?? 0;
      const cc = raw.cc ?? 0;
      for (let i = 0; i < cc; i++) {
        node.children.push(buildNode(cs + i));
      }
    } else {
      // Split signed deltas into magnitude + sign for the UI
      const d1raw = raw.d1 ?? 0;
      node.d1     = Math.abs(d1raw);
      node.d1sign = d1raw < 0 ? -1 : 1;
      const d2raw = raw.d2 ?? 0;
      node.d2     = Math.abs(d2raw);
      node.d2sign = d2raw < 0 ? -1 : 1;
    }
    delete node.cs;
    delete node.cc;
    return node;
  };

  return buildNode(0);
};

// Convert nested tree for UI → flat array for server (BFS to match C++ childStart/childCount indexing)
const flattenConditionTree = (rootNode) => {
  if (!rootNode) return [];

  const flat = [];
  const queue = [rootNode];

  while (queue.length > 0) {
    const current = queue.shift();
    const flatNode = { t: current.t };

    if (current.t === COND_CMP) {
      flatNode.src = current.src;
      flatNode.si  = current.si ?? 0;
      // Apply sign to magnitude before sending — C++ expects signed float
      flatNode.d1  = applySign(current.d1, current.d1sign);
      flatNode.op  = current.op;
      flatNode.ivs = current.ivs ?? false;
      if (current.ivs) {
        flatNode.src2 = current.src2 ?? SRC_TEMP;
        flatNode.si2  = current.si2 ?? 0;
        flatNode.d2   = applySign(current.d2, current.d2sign);
      } else {
        flatNode.v = current.v ?? 0;
      }
      flatNode.hyst = current.hyst ?? 0;
    } else {
      const children = current.children || [];
      flatNode.cs = flat.length + queue.length + 1;
      flatNode.cc = children.length;
      queue.push(...children);
    }

    flat.push(flatNode);
  }

  return flat;
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
              conditionTree: unflattenConditions(f.conditions),
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
      const out = { ...f, conditions: flattenConditionTree(f.conditionTree) };
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
