import React from 'react';
import ColumnBlock from '@/components/layout/ColumnBlock';
import Select from "@/components/ui/Select";
import SaveButton from '@/components/ui/SaveButton';
import DarkWrapperBlock from '@/components/layout/DarkWrapperBlock';
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
        <DarkWrapperBlock className='justify-between' key={index}>
          <p className="text-lg text-gray-300">
            Реле {index}: {decodeState(relay.isActive)}
          </p>
          <div className='flex gap-2 flex-col md:flex-row'>
            <Select
              value={relaysMode[index]?.mode}
              options={RELAY_OPTIONS}
              onChange={handleRelayModeChange(index)}
            />
            <SaveButton onClick={handleSetRelayMode(index)} />
          </div>
        </DarkWrapperBlock>
        ))}
      </div>
    </ColumnBlock>
  );
};

export default Relays;