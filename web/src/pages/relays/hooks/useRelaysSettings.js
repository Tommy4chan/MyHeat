import { useEffect, useState } from 'react';
import useRelayStore from '@/store/relayStore';
import usePinStore from '@/store/pinStore';

export const useRelaysSettings = () => {
  const { handlePinChange, getPins } = usePinStore();
  const {
    relaysSettings,
    getRelaysSettings,
    setRelaysSettings,
    getRelayCount,
    relayCount,
    setRelayCount,
  } = useRelayStore();

  const [relays, setRelays] = useState([]);
  const [localRelayCount, setLocalRelayCount] = useState(1);

  useEffect(() => {
    getRelayCount();
    getRelaysSettings();
    getPins();
  }, []);

  useEffect(() => {
    setLocalRelayCount(relayCount);
  }, [relayCount]);

  useEffect(() => {
    setRelays(relaysSettings);
  }, [relaysSettings]);

  const handleChangeRelayCount = (e) => {
    setLocalRelayCount(e.target.value);
  };

  const handleSetRelayCount = () => {
    setRelayCount(localRelayCount);
  };

  const handleRelayPinChange = (relayIndex) => (e) => {
    const newPin = parseInt(e.target.value);
    const oldPin = parseInt(relays[relayIndex].pin);

    const success = handlePinChange(oldPin, newPin);

    if (success) {
      const newRelays = [...relays];
      newRelays[relayIndex].pin = newPin;
      setRelays(newRelays);
    }
  };

  const handleIsActiveOnHighChange = (relayIndex) => (e) => {
    const newRelays = [...relays];
    newRelays[relayIndex].isActiveOnHigh = e.target.value;
    setRelays(newRelays);
  };

  const handleSaveRelaysSettings = () => {
    setRelaysSettings(relays);
  };

  return {
    relays,
    localRelayCount,
    handleChangeRelayCount,
    handleSetRelayCount,
    handleRelayPinChange,
    handleIsActiveOnHighChange,
    handleSaveRelaysSettings,
  };
};