import { useEffect, useState } from "react";
import Temperatures from "../components/Temperatures"
import DarkWrapperBlock from "../components/ui/DarkWrapperBlock";
import Button from "../components/ui/buttons/Button";
import Select from "../components/ui/Select";
import ColumnBlock from "../components/ui/ColumnBlock";
import SaveButton from "../components/ui/buttons/SaveButton";

const TemperaturesPage = () => {
  const [temperatures, setTemperatures] = useState([]);
  const [temperatureSensors, setTemperatureSensors] = useState([]);

  useEffect(() => {
    const fetchedTemperatures = [76.34, 72.12, 45.88, -127, -127, -3.12];

    setTemperatures(fetchedTemperatures);
    setTemperatureSensors([[40, 255, 6, 61, 132, 22, 3, 89], [39, 255, 2, 32, 211, 76, 9, 33]]);
  }, []);

  return (
    <div className="grid grid-cols-1 lg:grid-cols-2 items-start gap-6">
      <Temperatures isDeleteVisible={true} />
      <ColumnBlock>
        <h2 className="font-semibold text-2xl">Виявлені датчики</h2>
        {temperatureSensors?.map((temperatureSensor, index) => (
          <DarkWrapperBlock className="justify-between" key={index}>
            <p
              className="text-lg text-gray-300"
            >
              Датчик {index + 1}: {temperatureSensor.join(':')}
            </p>
            <div className="flex gap-2 flex-col md:flex-row">
              <Select
                defaultValue={0}
                options={Array.from({ length: temperatures.length }, (_, i) => ({ value: i, text: `T${i}` }))}
              />
              <SaveButton />
            </div>
          </DarkWrapperBlock>
        ))}
        <Button buttonText={'Виявити датчики'} color="indigo" />
      </ColumnBlock>
    </div>
  )
}

export default TemperaturesPage