import DarkWrapperBlock from "./layout/DarkWrapperBlock";
import Button from "./ui/Button";
import ColumnBlock from "./layout/ColumnBlock";
import useTemperatureStore from "@/store/temperatureStore";

const Temperatures = ({ isDeleteVisible = false }) => {
  const { temperatures, removeTemperatureSensor } = useTemperatureStore();

  return (
    <ColumnBlock>
      <h2 className="font-semibold text-2xl">Температура</h2>
      <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
        {temperatures?.map((temperature) => (
          <DarkWrapperBlock className="justify-between" key={temperature.id}>
            <p
              className="text-lg text-gray-300"
            >
              T{temperature.id}: {temperature.value === '-127.00' ? "Н\\Д" : `${temperature.value}°C`}
            </p>
            {isDeleteVisible && (
              <Button buttonText={'Видалити'} color="red" onClick={() => removeTemperatureSensor(temperature.id)}/>
            )}
          </DarkWrapperBlock>
        ))}
      </div>
    </ColumnBlock>
  );
};

export default Temperatures;