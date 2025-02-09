import { useState } from "react"
import DarkWrapperBlock from "../../../components/layout/DarkWrapperBlock"
import Input from "../../../components/ui/Input"
import Select from "../../../components/ui/Select"
import SettingsForm from "../../../components/layout/SettingsForm"
import SaveButton from "../../../components/ui/SaveButton"
import FormField from "../../../components/ui/FormField"
import WrapperBlock from "../../../components/layout/WrapperBlock"

const NTP = () => {
  const [ntpServer, setNtpServer] = useState('');
  const [timeZone, setTimeZone] = useState(Intl.DateTimeFormat().resolvedOptions().timeZone);

  const timeZoneOptions =  Intl.supportedValuesOf('timeZone').map(tz => ({
      value: tz,
      text: tz.replace('_', ' ')
    }));

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
              onChange={(e) => setTimeZone(e.target.value)}
              className={'w-full'}
            />
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>

      <SaveButton />
    </SettingsForm>
  )
}

export default NTP