import ColumnBlock from '@/components/layout/ColumnBlock';
import { RelayCountForm } from './RelaysCountForm';
import { RelayConfigList } from './RelayConfigList';
import { useRelaysSettings } from './hooks/useRelaysSettings';

export const RelaysSettings = () => {
  const {
    relays,
    localRelayCount,
    handleChangeRelayCount,
    handleSetRelayCount,
    handleRelayPinChange,
    handleIsActiveOnHighChange,
    handleSaveRelaysSettings,
  } = useRelaysSettings();

  return (
    <ColumnBlock>
      <h2 className="font-semibold text-2xl">Налаштування реле</h2>
      <RelayCountForm
        count={localRelayCount}
        onChange={handleChangeRelayCount}
        onSave={handleSetRelayCount}
      />
      <RelayConfigList
        relays={relays}
        onPinChange={handleRelayPinChange}
        onActiveHighChange={handleIsActiveOnHighChange}
        onSave={handleSaveRelaysSettings}
      />
    </ColumnBlock>
  );
};