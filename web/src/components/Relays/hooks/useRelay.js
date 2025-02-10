import { useEffect, useState } from 'react';
import useRelayStore from '@/store/relayStore';

const RELAY_STATES = {
  OFF: 0,
  ON: 1,
  AUTO: 2,
};

const RELAY_OPTIONS = [
  { value: RELAY_STATES.OFF, text: "Вимкнено" },
  { value: RELAY_STATES.ON, text: "Увімкнено" },
  { value: RELAY_STATES.AUTO, text: "Авто" },
];

export const useRelay = () => {
  const { relays, setRelayMode } = useRelayStore();
  const [relaysMode, setRelaysMode] = useState([]);

  const decodeState = (state) => {
    switch (Number(state)) {
      case RELAY_STATES.OFF:
        return "Вимкнено";
      case RELAY_STATES.ON:
        return "Увімкнено";
      case RELAY_STATES.AUTO:
        return "Авто";
      default:
        return "Невідомо";
    }
  };

  useEffect(() => {
    setRelaysMode(relays.map((relay, index) => {
      const currentMode = relaysMode[index];
      
      if (currentMode?.isChangedByUser) {
        if(currentMode.mode === relay.mode) {
          return { ...currentMode, isChangedByUser: false };
        }
        return currentMode;
      }
      
      return {
        mode: relay.mode,
        isChangedByUser: false
      };
    }));
  }, [relays]);

  const handleSetRelayMode = (index) => () => {
    setRelayMode(index, relaysMode[index].mode);
  };

  const handleRelayModeChange = (index) => (e) => {
    setRelaysMode(prev => prev.map((mode, i) => 
      i === index 
        ? { mode: e.target.value, isChangedByUser: true }
        : mode
    ));
  };

  return {
    relays,
    relaysMode,
    decodeState,
    handleSetRelayMode,
    handleRelayModeChange,
    RELAY_OPTIONS,
  };
};