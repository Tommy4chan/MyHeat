import DarkWrapperBlock from "@/components/layout/DarkWrapperBlock"
import Input from "@/components/ui/Input"
import Select from "@/components/ui/Select"
import SettingsForm from "@/components/layout/SettingsForm"
import SaveButton from "@/components/ui/SaveButton"
import FormField from "@/components/ui/FormField"
import WrapperBlock from "@/components/layout/WrapperBlock"
import { useNTPSettings } from "../hooks/useNTPSettings"

const NTP = () => {
  const {
    ntpServer,
    setNtpServer,
    timeZone,
    timeZoneOptions,
    handleChangeTimeZone,
    handleSaveNTPSettings
  } = useNTPSettings()

  return (
    <SettingsForm title='NTP'>
      <WrapperBlock>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label='Сервер'>
            <Input className='w-full' value={ntpServer} onChange={(e) => setNtpServer(e.target.value)} />
          </FormField>
          <FormField label='Часовий пояс'>
            <Select
              options={timeZoneOptions}
              value={timeZone}
              onChange={handleChangeTimeZone}
              className={'w-full'}
            />
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>

      <SaveButton onClick={handleSaveNTPSettings} />
    </SettingsForm>
  )
}

export default NTP