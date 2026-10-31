import { create } from "zustand";
import useWebSocketStore from "./websocketStore";
import { showWarningToast } from "@/components/CustomToast";

// Every controller serves its own UI, switching opens the other controller's page.
// The list lives in localStorage, which is per address, so it is carried to the next controller in the URL hash
const STORAGE_KEY = "myheat.controllers";
const HASH_KEY = "c=";
// Hash used by the first version of the switcher, still read so older pages can hand over their list
const LEGACY_HASH_KEY = "controllers=";
const SCAN_TIMEOUT = 10000;
const IPV4 = /^\d{1,3}(\.\d{1,3}){3}$/;

// A controller is saved with up to two addresses:
//   host  local address, "<mdns>.local" or an IP, opened as http://<host>
//   url   external address, e.g. a Cloudflare tunnel https://heat.example.com
// The controllers have no TLS, so the local address is always http, https only comes from the external one
const newId = () => Math.random().toString(36).slice(2, 10);

// "myheat" -> "myheat.local", IPs and *.local stay as they are, anything else is not a local address
export const normalizeHost = (input) => {
  const host = (input || "").trim().toLowerCase().replace(/^http:\/\//, "").replace(/\/+$/, "").replace(/\.$/, "");
  if (!host) return "";
  if (IPV4.test(host) || host.endsWith(".local")) return host;
  if (/^[a-z0-9-]+$/.test(host)) return `${host}.local`;
  return null;
};

const hostFromMdns = (mdns, ip) => (mdns ? `${mdns}.local` : ip || "");

// Puts what the user typed into the field it belongs to
export const parseAddress = (input) => {
  const value = (input || "").trim();
  if (!value) return null;
  const hasScheme = /^https?:\/\//i.test(value);
  let parsed;
  try {
    parsed = new URL(hasScheme ? value : `http://${value}`);
  } catch {
    return null;
  }
  const host = parsed.hostname.toLowerCase();
  if (parsed.protocol === "http:" && !parsed.port) {
    const localHost = normalizeHost(host);
    // A bare name without dots is taken as an mDNS name, a domain without a scheme is an external address
    if (localHost && (hasScheme || IPV4.test(host) || host.endsWith(".local") || !host.includes("."))) {
      return { host: localHost };
    }
  }
  // External addresses are served over https (e.g. a tunnel) unless http was typed
  if (!hasScheme) parsed.protocol = "https:";
  return { url: parsed.origin };
};

// Locally the host is used. On an https page (opened through a tunnel) http addresses are blocked
// by the browser, so the external address goes first
export const getOpenUrl = (controller) => {
  if (window.location.protocol === "https:" && controller.url) return controller.url;
  if (controller.host) return `http://${controller.host}`;
  return controller.url || "";
};

export const describeAddresses = (controller) =>
  [controller.host, controller.url?.replace(/^http:\/\//, "")].filter(Boolean).join(" · ");

const isSameController = (a, b) => (!!a.host && a.host === b.host) || (!!a.url && a.url === b.url);

// Converts entries from older versions ({ url, address } and { mdns, ip }) and drops anything unusable
const cleanController = (controller) => {
  if (!controller || typeof controller !== "object") return null;
  let host = "";
  let url = "";
  if ("host" in controller) {
    host = normalizeHost(controller.host) || "";
    url = controller.url || "";
  } else if ("mdns" in controller || "ip" in controller) {
    host = hostFromMdns(controller.mdns, controller.ip);
    url = controller.url || "";
  } else if (controller.url) {
    const parsed = parseAddress(controller.url) || {};
    host = parsed.host || controller.address || "";
    url = parsed.url || "";
  }
  if (!host && !url) return null;

  const result = { id: controller.id || newId(), name: controller.name || "", host, url };
  if (controller.isNameCustom) result.isNameCustom = true;
  if (!result.name) result.name = describeAddresses(result);
  return result;
};

// Addresses from the added entry fill in or refresh the saved one, a hand-typed name is never replaced
const mergeControllers = (base, added) => {
  const result = base.map((c) => ({ ...c }));
  for (const raw of added) {
    const controller = cleanController(raw);
    if (!controller) continue;
    const existing = result.find((c) => isSameController(c, controller));
    if (!existing) {
      result.push(controller);
      continue;
    }
    if (controller.host) existing.host = controller.host;
    if (controller.url) existing.url = controller.url;
    if (controller.isNameCustom) {
      existing.name = controller.name;
      existing.isNameCustom = true;
    } else if (controller.name && !existing.isNameCustom) {
      existing.name = controller.name;
    }
  }
  return result;
};

const readStoredControllers = () => {
  try {
    const parsed = JSON.parse(localStorage.getItem(STORAGE_KEY) || "[]");
    return Array.isArray(parsed) ? parsed : [];
  } catch {
    return [];
  }
};

const storeControllers = (controllers) => {
  try {
    localStorage.setItem(STORAGE_KEY, JSON.stringify(controllers));
  } catch {
    // Storage can be blocked, the list then only lasts for this page
  }
};

// base64url of the UTF-8 JSON keeps the hash short and free of %XX escapes, empty fields are left out
const encodeControllers = (controllers) => {
  const compact = controllers.map((c) => Object.fromEntries(Object.entries(c).filter(([key, value]) => value && key !== "id")));
  const bytes = new TextEncoder().encode(JSON.stringify(compact));
  let binary = "";
  bytes.forEach((byte) => (binary += String.fromCharCode(byte)));
  return btoa(binary).replace(/\+/g, "-").replace(/\//g, "_").replace(/=+$/, "");
};

const decodeControllers = (value) => {
  const binary = atob(value.replace(/-/g, "+").replace(/_/g, "/"));
  return JSON.parse(new TextDecoder().decode(Uint8Array.from(binary, (char) => char.charCodeAt(0))));
};

// The list brought over from the previous controller is merged once and removed from the address bar
const readCarriedControllers = () => {
  const hash = window.location.hash.slice(1);
  if (!hash.startsWith(HASH_KEY) && !hash.startsWith(LEGACY_HASH_KEY)) return [];
  window.history.replaceState(null, "", window.location.pathname + window.location.search);
  try {
    const parsed = hash.startsWith(HASH_KEY)
      ? decodeControllers(hash.slice(HASH_KEY.length))
      : JSON.parse(decodeURIComponent(hash.slice(LEGACY_HASH_KEY.length)));
    return Array.isArray(parsed) ? parsed : [];
  } catch {
    return [];
  }
};

const initialControllers = (() => {
  const merged = mergeControllers([], [...readStoredControllers(), ...readCarriedControllers()]);
  storeControllers(merged);
  return merged;
})();

let scanTimeout = null;

const useControllerStore = create((set, get) => ({
  controllers: initialControllers,
  scannedControllers: [],
  isScanning: false,
  // Name, mDNS and IP of the controller this page came from, sent by the firmware on connect
  deviceInfo: null,

  setControllers: (controllers) => {
    storeControllers(controllers);
    set({ controllers });
  },

  addController: (name, address) => {
    const parsed = parseAddress(address);
    if (!parsed) {
      showWarningToast("Невірна адреса контролера");
      return false;
    }
    const entry = { name: name.trim(), ...parsed, ...(name.trim() && { isNameCustom: true }) };
    get().setControllers(mergeControllers(get().controllers, [entry]));
    return true;
  },

  // Saves a found controller as it was reported, its name keeps following the device name
  saveController: (controller) => {
    get().setControllers(mergeControllers(get().controllers, [controller]));
  },

  updateController: (id, { name, host, url }) => {
    const entry = { host: normalizeHost(host), url: "" };

    if (entry.host === null) {
      showWarningToast("Локальна адреса: mDNS-ім'я (myheat.local) або IP");
      return false;
    }
    if ((url || "").trim()) {
      try {
        const value = url.trim();
        entry.url = new URL(/^https?:\/\//i.test(value) ? value : `https://${value}`).origin;
      } catch {
        showWarningToast("Невірна зовнішня адреса");
        return false;
      }
    }
    if (!entry.host && !entry.url) {
      showWarningToast("Вкажіть хоча б одну адресу");
      return false;
    }

    // An unchanged name keeps its state, a changed one is kept from now on instead of following the
    // controller's device name, a cleared one goes back to following it (until then the old name stays)
    const original = get().controllers.find((c) => c.id === id);
    const typedName = name.trim();
    const isNameChanged = !!typedName && typedName !== original?.name;
    const isNameCustom = isNameChanged || (!!typedName && !!original?.isNameCustom);
    const updated = {
      id,
      ...entry,
      name: typedName || original?.name || describeAddresses(entry),
      ...(isNameCustom && { isNameCustom: true }),
    };
    const others = get().controllers.filter((c) => c.id !== id && !isSameController(c, updated));
    const index = get().controllers.findIndex((c) => c.id === id);
    others.splice(Math.min(index, others.length), 0, updated);
    get().setControllers(others);
    return true;
  },

  removeController: (id) => {
    get().setControllers(get().controllers.filter((c) => c.id !== id));
  },

  // The same controller can be saved by its .local name, its IP or a tunnel address
  isCurrent: (controller) => {
    const { deviceInfo } = get();
    const origin = window.location.origin;
    if ((controller.host && `http://${controller.host}` === origin) || controller.url === origin) return true;
    if (!deviceInfo || !controller.host) return false;
    return controller.host === `${deviceInfo.mdns}.local` || controller.host === deviceInfo.address;
  },

  getCurrentEntry: () => {
    const { deviceInfo } = get();
    const fromPage = parseAddress(window.location.origin) || {};
    return {
      name: deviceInfo?.name || "",
      // mDNS name by default, the IP only if the controller has no mDNS name
      host: hostFromMdns(deviceInfo?.mdns, deviceInfo?.address) || fromPage.host || "",
      // A tunnel address is only known from the page itself
      url: fromPage.url || "",
    };
  },

  openController: (controller) => {
    // The current controller is remembered too, so the other one can switch back
    const list = mergeControllers(get().controllers, [get().getCurrentEntry(), controller]);
    get().setControllers(list);

    const target = getOpenUrl(controller);
    if (!target) return;
    window.location.href = `${target}/#${HASH_KEY}${encodeControllers(list)}`;
  },

  startDeviceScan: () => {
    set({ isScanning: true, scannedControllers: [] });
    useWebSocketStore.getState().sendMessage("startDeviceScan");

    clearTimeout(scanTimeout);
    scanTimeout = setTimeout(() => set({ isScanning: false }), SCAN_TIMEOUT);
  },

  processDeviceScanData: (payload) => {
    clearTimeout(scanTimeout);
    // Found controllers get their mDNS name as the local address, the IP only if they have none
    const scannedControllers = (payload || [])
      .map((device) => cleanController({ name: device.name, host: hostFromMdns(device.mdns, device.address) }))
      .filter(Boolean);
    set({ isScanning: false, scannedControllers });
  },

  processDeviceInfo: (payload) => {
    set({ deviceInfo: payload || null });
    if (!payload?.name) return;

    document.title = payload.name;

    // The opened controller is always in the list. Its saved entry is found by mDNS name or IP,
    // then follows what the controller reports
    const { controllers, isCurrent } = get();
    const current = get().getCurrentEntry();
    const existing = controllers.find((c) => isCurrent(c));
    const merged = existing
      ? controllers.map((c) => (c === existing ? { ...c, host: current.host || c.host, url: current.url || c.url, name: c.isNameCustom ? c.name : current.name } : c))
      : mergeControllers(controllers, [current]);
    if (JSON.stringify(merged) !== JSON.stringify(controllers)) {
      get().setControllers(merged);
    }
  },
}));

useWebSocketStore.subscribe(
  (state) => state.messages["deviceScanData"],
  (payload) => useControllerStore.getState().processDeviceScanData(payload)
);

useWebSocketStore.subscribe(
  (state) => state.messages["deviceInfo"],
  (payload) => useControllerStore.getState().processDeviceInfo(payload)
);

export default useControllerStore;
