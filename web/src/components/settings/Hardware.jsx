import { useState } from "react"
import BasicButton from "../ui/buttons/BasicButton";
import Input from "../ui/Input";
import ColumnBlock from "../ui/ColumnBlock";
import DarkWrapperBlock from "../ui/DarkWrapperBlock";
import BasicSelect from "../ui/BasicSelect";

const Hardware = () => {
  const [displayAddress, setDisplayAddress] = useState('');
  const [sda, setSda] = useState(0);
  const [scl, setScl] = useState(1);
  const [displaySleep, setDisplaySleep] = useState(0);

  const [encoderInvert, setEncoderInvert] = useState(false);
  const [encoderA, setEncoderA] = useState(2);
  const [encoderB, setEncoderB] = useState(3);
  const [encoderBtn, setEncoderBtn] = useState(4);

  const pins = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11];

  return (
    <ColumnBlock>
      <h2 className='font-semibold text-2xl'>Фізичний Ввід\Вивід</h2>
      <div className='w-full'>
            <label class='block mb-1 text-sm text-white'>Увімкнений:</label>
            <BasicSelect
              options={[
                { value: 'false', text: 'Ні' },
                { value: 'true', text: 'Так' }
              ]}
              className='w-full'
              color='light-gray'
            />
          </div>
      <div className='flex flex-col gap-2 w-full'>
        <h3 className='text-xl'>Екран:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <div className="flex w-full gap-2">
            <div className='w-full'>
              <label class='block mb-1 text-sm text-white'>Адреса екрану:</label>
              <Input className='w-full' value={displayAddress} onChange={(e) => setDisplayAddress(e.target.value)} />
            </div>
            <div className='w-full'>
              <label class='block mb-1 text-sm text-white'>Сон (секунди):</label>
              <Input className='w-full' value={displaySleep} onChange={(e) => setDisplaySleep(e.target.value)} isNumber={true} />
            </div>
          </div>
          <div className="flex w-full gap-2">
            <div className='w-full'>
              <label class='block mb-1 text-sm text-white'>SDA:</label>
              <BasicSelect
                value={sda}
                onChange={(e) => setSda(e.target.value)}
                options={Array.from({ length: pins.length }, (_, i) => ({ value: i, text: `D${i}` }))}
                className={'w-full'}
              />
            </div>
            <div className='w-full'>
              <label class='block mb-1 text-sm text-white'>SCL:</label>
              <BasicSelect
                value={scl}
                onChange={(e) => setScl(e.target.value)}
                options={Array.from({ length: pins.length }, (_, i) => ({ value: i, text: `D${i}` }))}
                className={'w-full'}
              />
            </div>
          </div>

        </DarkWrapperBlock>
      </div>

      <div className='flex flex-col gap-2 w-full'>
        <h3 className='text-xl'>Енкодер:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
        <div className='w-full'>
            <label class='block mb-1 text-sm text-white'>Інвертувати:</label>
            <BasicSelect
              value={encoderInvert}
              onChange={(e) => setEncoderInvert(e.target.value)}
              options={[
                { value: 'false', text: 'Ні' },
                { value: 'true', text: 'Так' }
              ]}
              className={'w-full'}
            />
          </div>
          <div className="flex w-full gap-2">
            <div className='w-full'>
              <label class='block mb-1 text-sm text-white'>A:</label>
              <BasicSelect
                value={encoderA}
                onChange={(e) => setEncoderA(e.target.value)}
                options={Array.from({ length: pins.length }, (_, i) => ({ value: i, text: `D${i}` }))}
                className={'w-full'}
              />
            </div>
            <div className='w-full'>
              <label class='block mb-1 text-sm text-white'>B:</label>
              <BasicSelect
                value={encoderB}
                onChange={(e) => setEncoderB(e.target.value)}
                options={Array.from({ length: pins.length }, (_, i) => ({ value: i, text: `D${i}` }))}
                className={'w-full'}
              />
            </div>
            <div className='w-full'>
              <label class='block mb-1 text-sm text-white'>BTN:</label>
              <BasicSelect
                value={encoderBtn}
                onChange={(e) => setEncoderBtn(e.target.value)}
                options={Array.from({ length: pins.length }, (_, i) => ({ value: i, text: `D${i}` }))}
                className={'w-full'}
              />
            </div>
          </div>

        </DarkWrapperBlock>
      </div>

      <BasicButton buttonText={'Зберегти'} color='purple' />
    </ColumnBlock>
  )
}

export default Hardware