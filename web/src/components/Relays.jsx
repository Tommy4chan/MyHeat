import Select from './ui/Select';
import DarkWrapperBlock from './ui/DarkWrapperBlock';
import ColumnBlock from './ui/ColumnBlock';
import SaveButton from './ui/buttons/SaveButton';
import useRelayStore from '../store/relayStore';

const Relays = () => {
  const { relays } = useRelayStore();

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
                value={relay.mode}
                options={[
                  { value: 0, text: "Вимкнено" },
                  { value: 1, text: "Увімкнено" },
                  { value: 2, text: "Авто" },
                ]}
              />
              <SaveButton />
            </div>

          </DarkWrapperBlock>
        ))}
      </div>
    </ColumnBlock>
  );
};

export default Relays;