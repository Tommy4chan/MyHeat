import { useEffect, useState } from "react";
import DarkWrapperBlock from "./ui/DarkWrapperBlock";
import BasicButton from "./ui/buttons/BasicButton";
import ColumnBlock from "./ui/ColumnBlock";

const Temperatures = ({ isDeleteVisible = false }) => {
  const [temperatures, setTemperatures] = useState([]);

  useEffect(() => {
    const fetchedTemperatures = [76.34, 72.12, 45.88, -127, -127, -3.12];
    const half = Math.ceil(fetchedTemperatures.length / 2);

    let formattedTemperatures = [];

    for (let i = 0; i < half; i++) {
      formattedTemperatures.push({ id: i, value: fetchedTemperatures[i] });
      if (half + i < fetchedTemperatures.length) {
        formattedTemperatures.push({ id: half + i, value: fetchedTemperatures[half + i] });
      }
    }

    setTemperatures(formattedTemperatures);
  }, []);

  return (
    <ColumnBlock>
      <h2 className="font-semibold text-2xl">Температура</h2>
      <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
        {temperatures?.map((temperature) => (
          <DarkWrapperBlock className="justify-between" key={temperature.id}>
            <p
              className="text-lg text-gray-300"
            >
              T{temperature.id}: {temperature.value === -127 ? "Н\\Д" : `${temperature.value}°C`}
            </p>
            {isDeleteVisible && (
              <BasicButton buttonText={'Видалити'} color="red" />
            )}
          </DarkWrapperBlock>
        ))}
      </div>
    </ColumnBlock>
  );
};

export default Temperatures