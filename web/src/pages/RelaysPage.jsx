import { useEffect, useState } from "react";
import Relays from "../components/Relays"
import Button from "../components/ui/buttons/Button"
import DarkWrapperBlock from "../components/ui/DarkWrapperBlock";
import Select from "../components/ui/Select";
import ColumnBlock from "../components/ui/ColumnBlock";
import usePinStore from "../store/pinStore";
import { handlePinChange } from "../utils/pinHandler";
import FormField from "../components/ui/FormField";

const RelaysPage = () => {
  const [relaysParameters, setRelaysParameters] = useState([]);
  const [relaysPins, setRelaysPins] = useState([]);

  const {
    getAvailableOutputPins,
  } = usePinStore();

  useEffect(() => {
    const fetchedRelays = [{ pin: 2, isActiveOnHigh: true }, { pin: 3, isActiveOnHigh: true }, { pin: 4, isActiveOnHigh: false }, { pin: 5, isActiveOnHigh: false }];

    setRelaysParameters(fetchedRelays);
    setRelaysPins(fetchedRelays.map((relay) => relay.pin));
  }, []);

  const handleRelayPinChange = (e, relayIndex) => {
    handlePinChange(e, setRelaysPins, relaysPins, relayIndex);
  }

  return (
    <div className="grid grid-cols-1 lg:grid-cols-2 items-start gap-6">
      <Relays />
      <ColumnBlock>
        <h2 className="font-semibold text-2xl">Налаштування реле</h2>
        <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
          {relaysParameters.map((relay, index) => (
            <DarkWrapperBlock className="md:!flex-col !items-start" key={index}>
              <p className="text-lg text-gray-300">
                Реле {index}:
              </p>
              <FormField label='Пін'>
                <Select
                  value={relaysPins[index]}
                  options={getAvailableOutputPins(relaysPins[index])}
                  onChange={(e) => handleRelayPinChange(e, index)}
                  className='w-full'
                />
              </FormField>
              <FormField label='Увімкнено при'>
                <Select
                  value={Number(relay.isActiveOnHigh)}
                  options={[
                    { value: 0, text: 'Low' },
                    { value: 1, text: 'High' },
                  ]}
                  className='w-full'
                />
              </FormField>
            </DarkWrapperBlock>
          ))}
        </div>
        <Button buttonText={'Зберегти'} color="purple" />
      </ColumnBlock>
    </div>
  )
}

export default RelaysPage