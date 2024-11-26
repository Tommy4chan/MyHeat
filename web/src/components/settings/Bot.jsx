import { useState } from "react"
import BasicButton from "../ui/buttons/BasicButton";
import Input from "../ui/Input";
import ColumnBlock from "../ui/ColumnBlock";
import DarkWrapperBlock from "../ui/DarkWrapperBlock";
import BasicSelect from "../ui/BasicSelect";

const Bot = () => {
  const [botToken, setBotToken] = useState('');
  const [botPassword, setBotPassword] = useState('');
  const [isBotEnabled, setIsBotEnabled] = useState(false);

  return (
    <ColumnBlock>
      <h2 className='font-semibold text-2xl'>Телеграм бот</h2>
      <div className='flex flex-col gap-2 w-full'>
        <DarkWrapperBlock className='md:!flex-col'>
          <div className='w-full'>
            <label class='block mb-1 text-sm text-white'>Токен:</label>
            <Input className='w-full' value={botToken} onChange={(e) => setBotToken(e.target.value)} />
          </div>
          <div className='w-full'>
            <label class='block mb-1 text-sm text-white'>Фраза реєстрації:</label>
            <Input className='w-full' value={botPassword} onChange={(e) => setBotPassword(e.target.value)} />
          </div>
          <div className='w-full'>
            <label class='block mb-1 text-sm text-white'>Увімкнений:</label>
            <BasicSelect
              options={[
                { value: 'false', text: 'Ні' },
                { value: 'true', text: 'Так' }
              ]}
              value={isBotEnabled}
              onChange={(e) => setIsBotEnabled(e.target.value)}
              className={'w-full'}
            />
          </div>
        </DarkWrapperBlock>
      </div>

      <BasicButton buttonText={'Зберегти'} color='purple' />
    </ColumnBlock>
  )
}

export default Bot