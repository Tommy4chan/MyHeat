import { useState } from "react"
import Input from "../ui/Input";
import DarkWrapperBlock from "../ui/DarkWrapperBlock";
import Select from "../ui/Select";
import usePinStore from "../../store/pinStore";
import { handlePinChange } from "../../utils/pinHandler";
import FormColumn from "../ui/FormColumn";
import SaveButton from "../ui/buttons/SaveButton";
import SelectToggle from "../ui/SelectToggle";
import FormField from "../ui/FormField";

const Hardware = () => {
  const {
    getAvailableInputPins,
    getAvailableOutputPins,
  } = usePinStore();

  const [displayAddress, setDisplayAddress] = useState('');
  const [i2c, setI2c] = useState([12, 13]);
  const [displaySleep, setDisplaySleep] = useState(0);

  const [encoderInvert, setEncoderInvert] = useState(false);
  const [encoder, setEncoder] = useState([14, 15, 22]);

  const handleI2cPinChange = (e, pinIndex) => {
    handlePinChange(e, setI2c, i2c, pinIndex);
  }

  const handleEncoderPinChange = (e, pinIndex) => {
    handlePinChange(e, setEncoder, encoder, pinIndex);
  }

  return (
    <FormColumn title='Фізичний Ввід\Вивід'>
      <FormField label='Увімкнений'>
        <SelectToggle
          value={true}
          onChange={(e) => { }}
          className='w-full'
          color="light-gray"
        />
      </FormField>
      <div className='flex flex-col gap-2 w-full'>
        <h3 className='text-xl'>Екран:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <div className="flex w-full gap-2">
            <FormField label='Адреса екрану'>
              <Input className='w-full' value={displayAddress} onChange={(e) => setDisplayAddress(e.target.value)} />
            </FormField>
            <FormField label='Сон (секунди)'>
              <Input className='w-full' value={displaySleep} onChange={(e) => setDisplaySleep(e.target.value)} isNumber={true} />
            </FormField>
          </div>
          <div className="flex w-full gap-2">
            <FormField label='SDA'>
              <Select
                value={i2c[0]}
                options={getAvailableOutputPins(i2c[0])}
                onChange={(e) => handleI2cPinChange(e, 0)}
                className={'w-full'}
              />
            </FormField>
            <FormField label='SCL'>
              <Select
                value={i2c[1]}
                options={getAvailableOutputPins(i2c[1])}
                onChange={(e) => handleI2cPinChange(e, 1)}
                className={'w-full'}
              />
            </FormField>
          </div>

        </DarkWrapperBlock>
      </div>

      <div className='flex flex-col gap-2 w-full'>
        <h3 className='text-xl'>Енкодер:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label='Інвертувати'>
            <SelectToggle
              value={encoderInvert}
              onChange={(e) => setEncoderInvert(e.target.value)}
              className='w-full'
            />
          </FormField>
          <div className="flex w-full gap-2">
            <FormField label='CLK'>
              <Select
                value={encoder[0]}
                onChange={(e) => handleEncoderPinChange(e, 0)}
                options={getAvailableInputPins(encoder[0])}
                className={'w-full'}
              />
            </FormField>
            <FormField label='DT'>
              <Select
                value={encoder[1]}
                onChange={(e) => handleEncoderPinChange(e, 1)}
                options={getAvailableInputPins(encoder[1])}
                className={'w-full'}
              />
            </FormField>
            <FormField label='SW'>
              <Select
                value={encoder[2]}
                onChange={(e) => handleEncoderPinChange(e, 2)}
                options={getAvailableInputPins(encoder[2])}
                className={'w-full'}
              />
            </FormField>
          </div>

        </DarkWrapperBlock>
      </div>

      <SaveButton />
    </FormColumn>
  )
}

export default Hardware