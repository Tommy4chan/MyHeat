import { useEffect, useState } from "react"
import Input from "../../../components/ui/Input";
import DarkWrapperBlock from "../../../components/layout/DarkWrapperBlock";
import SaveButton from "../../../components/ui/SaveButton";
import SettingsForm from "../../../components/layout/SettingsForm";
import FormField from "../../../components/ui/FormField";
import SelectToggle from "../../../components/ui/SelectToggle";
import WrapperBlock from "../../../components/layout/WrapperBlock";
import useSettingStore from "../../../store/settingStore";

const Bot = () => {
  const [botToken, setBotToken] = useState('');
  const [botPassword, setBotPassword] = useState('');
  const [isBotEnabled, setIsBotEnabled] = useState(false);

  const { setTelegramBotSettings, getTelegramBotSettings, telegramBotSettings } = useSettingStore();

  useEffect(() => {
    getTelegramBotSettings();
  }, []);

  useEffect(() => {
    setBotToken(telegramBotSettings.token);
    setBotPassword(telegramBotSettings.registerPhrase);
    setIsBotEnabled(telegramBotSettings.isActive);
  }, [telegramBotSettings]);

  return (
    <SettingsForm title='Телеграм бот'>
      <WrapperBlock>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label='Токен'>
            <Input className='w-full' value={botToken} onChange={(e) => setBotToken(e.target.value)} />
          </FormField>
          <FormField label='Фраза реєстрації'>
            <Input className='w-full' value={botPassword} onChange={(e) => setBotPassword(e.target.value)} />
          </FormField>
          <FormField label='Увімкнений'>
            <SelectToggle
              value={isBotEnabled}
              onChange={(e) => setIsBotEnabled(e.target.value)}
              className='w-full'
            />
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>
      <SaveButton onClick={() => setTelegramBotSettings(botToken, botPassword, isBotEnabled)} />
    </SettingsForm>
  )
}

export default Bot