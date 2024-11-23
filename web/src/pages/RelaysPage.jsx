import { useEffect, useState } from "react";
import Relays from "../components/Relays"
import BasicButton from "../components/ui/buttons/BasicButton"
import DarkWrapperBlock from "../components/ui/DarkWrapperBlock";
import BasicSelect from "../components/ui/BasicSelect";
import ColumnBlock from "../components/ui/ColumnBlock";

const RelaysPage = () => {
  const [relays, setRelays] = useState([]);

  const pins = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11];

  useEffect(() => {
    const fetchedRelays = [{ pin: 2, isActiveOnHigh: true }, { pin: 3, isActiveOnHigh: true }, { pin: 4, isActiveOnHigh: false }, { pin: 5, isActiveOnHigh: false }];

    setRelays(fetchedRelays);
  }, []);


  return (
    <div className="flex items-start gap-6">
      <Relays />
      <ColumnBlock>
        <h2 className="font-semibold text-2xl">Налаштування реле</h2>
        <div className="grid grid-cols-2 gap-4">
        {relays?.map((relay, index) => (
          <DarkWrapperBlock className="flex-1 flex-col !items-start" key={index}>
            <p
              className="text-lg text-gray-300"
            >
              Реле {index + 1}:
            </p>
            <div className="flex gap-2">
              <p className="text-lg text-gray-300">Пін: </p>
              <BasicSelect
                defaultValue={pins.indexOf(relay.pin)}
                options={Array.from({ length: pins.length }, (_, i) => ({ value: i, text: `D${i}` }))}
              />
            </div>
            <div className="flex gap-2">
              <p className="text-lg text-gray-300">Увімкнено при: </p>
              <BasicSelect
                value={Number(relay.isActiveOnHigh)}
                options={[
                  { value: 0, text: 'Low' },
                  { value: 1, text: 'High' },
                ]}
              />
            </div>
          </DarkWrapperBlock>
        ))}
        </div>
        <BasicButton buttonText={'Зберегти'} color="purple" />
      </ColumnBlock>
    </div>
  )
}

export default RelaysPage