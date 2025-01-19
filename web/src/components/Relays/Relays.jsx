import React from 'react';
import ColumnBlock from '@/components/layout/ColumnBlock';
import { RelayItem } from './components/RelayItem';
import { useRelay } from './hooks/useRelay';

const Relays = () => {
  const {
    relays,
    relaysMode,
    decodeState,
    handleSetRelayMode,
    handleRelayModeChange,
    RELAY_OPTIONS,
  } = useRelay();

  return (
    <ColumnBlock>
      <h2 className="font-semibold text-2xl">Реле</h2>
      <div className="flex flex-col gap-4">
        {relays?.map((relay, index) => (
          <RelayItem
            key={index}
            index={index}
            isActive={relay.isActive}
            mode={relaysMode[index]?.mode}
            options={RELAY_OPTIONS}
            onModeChange={handleRelayModeChange(index)}
            onSave={handleSetRelayMode(index)}
            decodeState={decodeState}
          />
        ))}
      </div>
    </ColumnBlock>
  );
};

export default Relays;