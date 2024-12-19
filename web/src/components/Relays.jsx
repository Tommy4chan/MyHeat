import Select from './ui/Select';
import DarkWrapperBlock from './ui/DarkWrapperBlock';
import ColumnBlock from './ui/ColumnBlock';
import SaveButton from './ui/buttons/SaveButton';
import useRelayStore from '../store/relayStore';
import { useEffect, useState } from 'react';

const Relays = () => {
  const { relays, setRelayMode } = useRelayStore();
  const [relaysMode, setRelaysMode] = useState([]);

  const decode = (state) => {
    switch (Number(state)) {
      case 0:
        return "Вимкнено";
      case 1:
        return "Увімкнено";
      case 2:
        return "Авто";
    }
  }

  useEffect(() => {
    const newRelaysMode = relays.map((relay, index) => {
      const currentMode = relaysMode[index];
      
      if (currentMode?.isChangedByUser) {
        if(currentMode.mode === relay.mode) {
          return {...currentMode, isChangedByUser: false};
        }
        return currentMode;
      }
      
      return {
        mode: relay.mode,
        isChangedByUser: false
      };
    });

    setRelaysMode(newRelaysMode);
  }, [relays]);

  const handleSetRelayMode = (index) => () => {
    setRelayMode(index, relaysMode[index].mode);
  }

  const handleRelayModeChange = (index) => (e) => {
    const newRelaysMode = [...relaysMode];
    newRelaysMode[index].mode = e.target.value;
    newRelaysMode[index].isChangedByUser = true;
    setRelaysMode(newRelaysMode);
  }

  return (
    <ColumnBlock>
      <h2 className="font-semibold text-2xl">Реле</h2>

      <div className="flex flex-col gap-4">
        {relays?.map((relay, index) => (
          <DarkWrapperBlock className='justify-between' key={index}>
            <p className="text-lg text-gray-300">
              Реле {index}: {decode(relay.isActive)}
            </p>
            <div className='flex gap-2 flex-col md:flex-row'>
              <Select
                value={relaysMode[index]?.mode}
                options={[
                  { value: 0, text: "Вимкнено" },
                  { value: 1, text: "Увімкнено" },
                  { value: 2, text: "Авто" },
                ]}
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