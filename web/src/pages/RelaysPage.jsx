import { useEffect, useState } from "react";
import Relays from "../components/Relays"
import DarkWrapperBlock from "../components/ui/DarkWrapperBlock";
import Select from "../components/ui/Select";
import ColumnBlock from "../components/ui/ColumnBlock";
import usePinStore from "../store/pinStore";
import FormField from "../components/ui/FormField";
import Input from "../components/ui/Input";
import SaveButton from "../components/ui/buttons/SaveButton";
import useRelayStore from "../store/relayStore";

const RelaysPage = () => {
  const {
    getAvailableOutputPins,
    handlePinChange,
  } = usePinStore();

  const { relaysSettings, getRelaysSettings, setRelaysSettings, getRelayCount, relayCount, setRelayCount } = useRelayStore();

  const [relays, setRelays] = useState([]);
  const [localRelayCount, setLocalRelayCount] = useState(1);

  useEffect(() => {
    getRelayCount();
    getRelaysSettings();
  }, []);

  useEffect(() => {
    setLocalRelayCount(relayCount);
  }, [relayCount]);

  useEffect(() => {
    setRelays(relaysSettings);
  }, [relaysSettings]);

  const handleChangeRelayCount = (e) => {
    setLocalRelayCount(e.target.value);
  }

  const handleSetRelayCount = () => {
    setRelayCount(localRelayCount);
  }

  const handleRelayPinChange = (relayIndex) => (e) => {
    const newPin = parseInt(e.target.value);
    const oldPin = parseInt(relays[relayIndex].pin);
  
    const success = handlePinChange(oldPin, newPin);
  
    if (success) {
      const newRelays = [...relays];
      newRelays[relayIndex].pin = newPin;
      setRelays(newRelays);
    }
  }

  const handleIsActiveOnHighChange = (relayIndex) => (e) => {
    const newRelays = [...relays];
    newRelays[relayIndex].isActiveOnHigh = e.target.value;
    setRelays(newRelays);
  }

  const handleSaveRelaysSettings = () => {
    setRelaysSettings(relays);
  }

  return (
    <div className="grid grid-cols-1 lg:grid-cols-2 items-start gap-6">
      <Relays />
      <ColumnBlock>
        <h2 className="font-semibold text-2xl">Налаштування реле</h2>
        <div className="flex flex-col gap-2">
          <DarkWrapperBlock className="md:!flex-col">
            <FormField label='Кількість реле'>
              <Input
                value={localRelayCount}
                onChange={handleChangeRelayCount}
                isNumber={true}
                maxLength={2}
                className='w-full'
              />
            </FormField>
          </DarkWrapperBlock>
          <SaveButton onClick={handleSetRelayCount} />
        </div>
        <div className="flex flex-col gap-2">
          <div className="grid grid-cols-1 md:grid-cols-2 gap-2">
            {relays?.map((relay, index) => (
              <DarkWrapperBlock className="md:!flex-col !items-start" key={index}>
                <p className="text-lg text-gray-300">
                  Реле {index}:
                </p>
                <FormField label='Пін'>
                  <Select
                    value={relay.pin}
                    options={getAvailableOutputPins(relay.pin)}
                    onChange={handleRelayPinChange(index)}
                    className='w-full'
                  />
                </FormField>
                <FormField label='Увімкнено при'>
                  <Select
                    value={relay.isActiveOnHigh}
                    options={[
                      { value: 0, text: 'Low' },
                      { value: 1, text: 'High' },
                    ]}
                    onChange={handleIsActiveOnHighChange(index)}
                    className='w-full'
                  />
                </FormField>
              </DarkWrapperBlock>
            ))}
          </div>
          <SaveButton onClick={handleSaveRelaysSettings}/>
        </div>
      </ColumnBlock>
    </div>
  )
}

export default RelaysPage