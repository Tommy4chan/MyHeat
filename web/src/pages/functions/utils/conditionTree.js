import { COND_AND, COND_OR, COND_NOT, COND_CMP, SRC_TEMP, OP_LT, ACT_SET_RELAY } from "@/utils/functionEnums";

// Apply sign to magnitude before sending to C++
const applySign = (mag, sign) => Math.abs(mag ?? 0) * (sign ?? 1);

// Default empty node — matches what C++ expects when ivs=false
// When ivs=false, the constant value is serialized as "v", not "d2"
export const EMPTY_CMP_NODE = {
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

export const EMPTY_ACTION = { t: ACT_SET_RELAY, ti: 0, v: 1, d: 60, tt: 0 };

export const newCondition = () => ({ ...EMPTY_CMP_NODE, children: [] });
export const newGroup = () => ({ t: COND_AND, children: [newCondition()] });
export const isGroup = (node) => node?.t === COND_AND || node?.t === COND_OR;

// Convert flat conditions array from server → nested tree for UI
export const unflattenConditions = (flatArray) => {
  if (!flatArray || flatArray.length === 0) return newCondition();

  const buildNode = (index) => {
    if (index < 0 || index >= flatArray.length) return newCondition();
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
export const flattenConditionTree = (rootNode) => {
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

// Drops NOT nodes without a child, collapses double NOT and empty groups
const cleanNode = (node) => {
  if (!node) return null;
  if (node.t === COND_NOT) {
    const child = cleanNode(node.children?.[0]);
    if (!child) return null;
    return child.t === COND_NOT ? child.children[0] : { t: COND_NOT, children: [child] };
  }
  if (isGroup(node)) {
    const children = (node.children || []).map(cleanNode).filter(Boolean);
    return children.length ? { ...node, children } : null;
  }
  return node;
};

// Editor always works with a group at the root, so adding a second condition needs no type switching
export const toEditorTree = (flatArray) => {
  const root = cleanNode(unflattenConditions(flatArray)) ?? newCondition();
  return isGroup(root) ? root : { t: COND_AND, children: [root] };
};

// A root group with one condition is saved as that condition, same as before the editor wrapped it
export const fromEditorTree = (root) => {
  let node = cleanNode(root) ?? newCondition();
  if (isGroup(node) && node.children.length === 1) node = node.children[0];
  return flattenConditionTree(node);
};
