import Input from "@/components/ui/Input";
import DarkWrapperBlock from "@/components/layout/DarkWrapperBlock";
import Select from "@/components/ui/Select";
import usePinStore from "@/store/pinStore";
import SettingsForm from "@/components/layout/SettingsForm";
import SaveButton from "@/components/ui/SaveButton";
import SelectToggle from "@/components/ui/SelectToggle";
import FormField from "@/components/ui/FormField";
import WrapperBlock from "@/components/layout/WrapperBlock";
import { useHardwareSettings } from "../hooks/useHardwareSettings";

const Hardware = () => {
  const {
    getAvailableInputPins,
    getAvailableOutputPins,
  } = usePinStore();

  const {
    isEnabled,
    setIsEnabled,
    oledAddress,
    setOledAddress,
    screenPowerSaveInterval,
    setScreenPowerSaveInterval,
    encInvert,
    setEncInvert,
    encoderPins,
    setEncoderPins,
    oledPins,
    setOledPins,
    handleHardwarePinChange,
    handleSaveHardwareIOSettings,
  } = useHardwareSettings();

  return (
    <SettingsForm title='Фізичний Ввід\Вивід'>
      <FormField label='Увімкнений'>
        <SelectToggle
          value={isEnabled}
          onChange={(e) => setIsEnabled(e.target.value)}
          className='w-full'
          color="light-gray"
        />
      </FormField>
      <WrapperBlock>
        <h3 className='text-xl'>Екран:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <div className="flex w-full gap-2">
            <FormField label='Адреса екрану'>
              <Input className='w-full' value={oledAddress} onChange={(e) => setOledAddress(e.target.value)} />
            </FormField>
            <FormField label='Сон (секунди)'>
              <Input className='w-full' value={screenPowerSaveInterval} onChange={(e) => setScreenPowerSaveInterval(e.target.value)} isNumber={true} />
            </FormField>
          </div>
          <div className="flex w-full gap-2">
            <FormField label='SDA'>
              <Select
                value={oledPins[0]}
                options={getAvailableOutputPins(oledPins[0])}
                onChange={handleHardwarePinChange(0, oledPins, setOledPins)}
                className={'w-full'}
              />
            </FormField>
            <FormField label='SCL'>
              <Select
                value={oledPins[1]}
                options={getAvailableOutputPins(oledPins[1])}
                onChange={handleHardwarePinChange(1, oledPins, setOledPins)}
                className={'w-full'}
              />
            </FormField>
          </div>
        </DarkWrapperBlock>
      </WrapperBlock>

      <WrapperBlock>
        <h3 className='text-xl'>Енкодер:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label='Інвертувати'>
            <SelectToggle
              value={encInvert}
              onChange={(e) => setEncInvert(e.target.value)}
              className='w-full'
            />
          </FormField>
          <div className="flex w-full gap-2">
            <FormField label='CLK'>
              <Select
                value={encoderPins[0]}
                onChange={handleHardwarePinChange(0, encoderPins, setEncoderPins)}
                options={getAvailableInputPins(encoderPins[0])}
                className={'w-full'}
              />
            </FormField>
            <FormField label='DT'>
              <Select
                value={encoderPins[1]}
                onChange={handleHardwarePinChange(1, encoderPins, setEncoderPins)}
                options={getAvailableInputPins(encoderPins[1])}
                className={'w-full'}
              />
            </FormField>
            <FormField label='SW'>
              <Select
                value={encoderPins[2]}
                onChange={handleHardwarePinChange(2, encoderPins, setEncoderPins)}
                options={getAvailableInputPins(encoderPins[2])}
                className={'w-full'}
              />
            </FormField>
          </div>

        </DarkWrapperBlock>
      </WrapperBlock>

      <SaveButton onClick={handleSaveHardwareIOSettings} />
    </SettingsForm>
  )
}

export default Hardware