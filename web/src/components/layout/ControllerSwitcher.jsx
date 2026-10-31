import { useEffect, useRef, useState } from "react";
import { MdClose, MdExpandMore, MdAdd, MdEdit } from "react-icons/md";
import Button from "@/components/ui/Button";
import Input from "@/components/ui/Input";
import useControllerStore, { describeAddresses } from "@/store/controllerStore";

// The popup is gray-800, the same color the shared Input uses for hover and its focus ring
const popupInputClass = "hover:!bg-gray-900 focus:!ring-gray-600";

const iconButtonClass = "p-1 rounded text-gray-500 hover:text-gray-200 hover:bg-gray-600";

// The whole row opens the controller, the icon buttons only do their own action
const ControllerRow = ({ controller, isCurrent, onOpen, onAdd, onEdit, onRemove }) => {
  const stop = (action) => (e) => {
    e.stopPropagation();
    action();
  };

  return (
    <div
      onClick={isCurrent ? undefined : onOpen}
      title={isCurrent ? undefined : 'Відкрити'}
      className={`flex items-center gap-2 px-3 py-2 rounded ${isCurrent ? 'bg-gray-900 border-l-2 border-indigo-500' : 'hover:bg-gray-700 cursor-pointer'}`}
    >
      <div className="flex-1 min-w-0">
        <p className="truncate">{controller.name}</p>
        <p className="text-xs text-gray-500 truncate">{describeAddresses(controller)}</p>
      </div>
      {isCurrent && <span className="text-xs text-indigo-400">поточний</span>}
      {onAdd && (
        <button type="button" title="Зберегти в список" onClick={stop(onAdd)} className={iconButtonClass}>
          <MdAdd size={18} />
        </button>
      )}
      {onEdit && (
        <button type="button" title="Редагувати" onClick={stop(onEdit)} className={iconButtonClass}>
          <MdEdit size={18} />
        </button>
      )}
      {onRemove && (
        <button type="button" title="Видалити зі списку" onClick={stop(onRemove)} className="p-1 rounded text-gray-500 hover:text-red-400 hover:bg-gray-600">
          <MdClose size={18} />
        </button>
      )}
    </div>
  );
};

const EditField = ({ label, children }) => (
  <label className="flex flex-col gap-1 text-xs text-gray-500">
    {label}
    {children}
  </label>
);

const ControllerEditRow = ({ controller, onSave, onCancel }) => {
  const [fields, setFields] = useState({
    name: controller.name,
    host: controller.host,
    url: controller.url,
  });
  const change = (e) => setFields((prev) => ({ ...prev, [e.target.name]: e.target.value }));

  return (
    <div className="flex flex-col gap-2 px-3 py-2 rounded bg-gray-900">
      <EditField label="Назва">
        <Input className="w-full text-sm" name="name" value={fields.name} onChange={change} placeholder={controller.name} />
      </EditField>
      <EditField label="Локальна адреса">
        <Input className="w-full text-sm" name="host" value={fields.host} onChange={change} placeholder="myheat.local або 192.168.1.50" />
      </EditField>
      <EditField label="Зовнішня адреса">
        <Input className="w-full text-sm" name="url" value={fields.url} onChange={change} placeholder="https://heat.example.com" />
      </EditField>
      <p className="text-xs text-gray-500">
        Локальна адреса відкривається через http. Зовнішня використовується, коли сторінка відкрита через https, наприклад тунель.
      </p>
      <div className="flex gap-2 justify-end">
        <Button buttonText="Скасувати" onClick={onCancel} className="text-sm" />
        <Button buttonText="Зберегти" color="indigo" onClick={() => onSave(fields)} className="text-sm" />
      </div>
    </div>
  );
};

const ControllerSwitcher = () => {
  const {
    controllers, scannedControllers, isScanning, deviceInfo,
    addController, saveController, updateController, removeController, openController, startDeviceScan, isCurrent,
  } = useControllerStore();
  const [isOpen, setIsOpen] = useState(false);
  const [editingId, setEditingId] = useState(null);
  const [name, setName] = useState('');
  const [address, setAddress] = useState('');
  const panelRef = useRef(null);

  useEffect(() => {
    if (!isOpen) return;
    const handleClick = (e) => {
      if (panelRef.current && !panelRef.current.contains(e.target)) setIsOpen(false);
    };
    document.addEventListener('mousedown', handleClick);
    return () => document.removeEventListener('mousedown', handleClick);
  }, [isOpen]);

  const handleAdd = () => {
    if (addController(name, address)) {
      setName('');
      setAddress('');
    }
  };

  // Found controllers that are already saved are not repeated
  const newScanned = scannedControllers.filter((s) => !controllers.some((c) => c.host === s.host));
  const currentName = deviceInfo?.name || window.location.host;
  // The current controller is always shown first, the others keep their saved order
  const sortedControllers = [...controllers].sort((a, b) => isCurrent(b) - isCurrent(a));

  return (
    <div className="relative" ref={panelRef}>
      <button
        type="button"
        onClick={() => setIsOpen(!isOpen)}
        className="flex items-center gap-1 px-2 py-1 rounded text-sm text-gray-400 hover:text-gray-200 hover:bg-gray-700 transition-colors max-w-[9rem] sm:max-w-[14rem]"
        title="Перемкнути контролер"
      >
        <span className="truncate">{currentName}</span>
        <MdExpandMore size={18} className={`shrink-0 transition-transform ${isOpen ? 'rotate-180' : ''}`} />
      </button>

      {isOpen && (
        <div className="absolute left-0 top-full mt-2 z-50 w-80 max-w-[calc(100vw-2rem)] bg-gray-800 border border-gray-700 rounded shadow-lg p-3 flex flex-col gap-3">
          <div className="flex flex-col gap-1">
            <p className="text-xs uppercase text-gray-500">Контролери</p>
            {controllers.length === 0 && (
              <p className="text-sm text-gray-500 px-3 py-2">Список порожній. Знайдіть контролери в мережі або додайте адресу вручну.</p>
            )}
            {sortedControllers.map((controller) => {
              const isCurrentController = isCurrent(controller);
              return editingId === controller.id ? (
                <ControllerEditRow
                  key={controller.id}
                  controller={controller}
                  onCancel={() => setEditingId(null)}
                  onSave={(fields) => {
                    if (updateController(controller.id, fields)) setEditingId(null);
                  }}
                />
              ) : (
                <ControllerRow
                  key={controller.id}
                  controller={controller}
                  isCurrent={isCurrentController}
                  onOpen={() => openController(controller)}
                  // The current controller reports its own name and addresses and is re-added on every load,
                  // so it is neither edited nor removed here
                  onEdit={isCurrentController ? undefined : () => setEditingId(controller.id)}
                  onRemove={isCurrentController ? undefined : () => removeController(controller.id)}
                />
              );
            })}
          </div>

          <div className="flex flex-col gap-1">
            <div className="flex items-center justify-between">
              <p className="text-xs uppercase text-gray-500">У мережі</p>
              <Button buttonText={isScanning ? 'Пошук...' : 'Пошук'} color="indigo" className="text-sm" onClick={isScanning ? undefined : startDeviceScan} />
            </div>
            {newScanned.map((controller) => (
              <ControllerRow
                key={controller.id}
                controller={controller}
                isCurrent={false}
                onOpen={() => openController(controller)}
                onAdd={() => saveController(controller)}
              />
            ))}
            {!isScanning && scannedControllers.length > 0 && newScanned.length === 0 && (
              <p className="text-sm text-gray-500 px-3">Усі знайдені контролери вже в списку</p>
            )}
          </div>

          <div className="flex flex-col gap-2 border-t border-gray-700 pt-3">
            <p className="text-xs uppercase text-gray-500">Додати вручну</p>
            <Input className={`w-full text-sm ${popupInputClass}`} value={name} onChange={(e) => setName(e.target.value)} placeholder="Назва (необов'язково)" />
            <div className="flex gap-2">
              <Input className={`flex-1 min-w-0 text-sm ${popupInputClass}`} value={address} onChange={(e) => setAddress(e.target.value)} placeholder="myheat.local, 192.168.1.50 або https://..." />
              <Button buttonText="Додати" onClick={handleAdd} className="text-sm" />
            </div>
          </div>
        </div>
      )}
    </div>
  );
};

export default ControllerSwitcher;
