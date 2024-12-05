import { useMemo, useState } from "react"
import DarkWrapperBlock from "../ui/DarkWrapperBlock"
import Input from "../ui/Input"
import Select from "../ui/Select"
import FormColumn from "../ui/FormColumn"
import SaveButton from "../ui/buttons/SaveButton"
import FormField from "../ui/FormField"
import WrapperBlock from "../ui/WrapperBlock"

const NTP = () => {
  const [ntpServer, setNtpServer] = useState('');
  const [timeZone, setTimeZone] = useState(Intl.DateTimeFormat().resolvedOptions().timeZone);

  const timeZoneOptions = useMemo(() => {
    return Intl.supportedValuesOf('timeZone').map(tz => ({
      value: tz,
      text: tz.replace('_', ' ')
    }));
  }, []);

  return (
    <FormColumn title='NTP'>
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
    </FormColumn>
  )
}

export default NTP