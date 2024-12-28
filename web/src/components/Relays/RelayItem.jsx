import React, { memo } from 'react';
import Select from "../ui/Select";
import SaveButton from '../ui/SaveButton';
import DarkWrapperBlock from '../layout/DarkWrapperBlock';

export const RelayItem = memo(({
  index,
  isActive,
  mode,
  options,
  onModeChange,
  onSave,
  decodeState
}) => {
  return (
    <DarkWrapperBlock className='justify-between'>
      <p className="text-lg text-gray-300">
        Реле {index}: {decodeState(isActive)}
      </p>
      <div className='flex gap-2 flex-col md:flex-row'>
        <Select
          value={mode}
          options={options}
          onChange={onModeChange}
        />
        <SaveButton onClick={onSave} />
      </div>
    </DarkWrapperBlock>
  );
});