import Input from "../../../components/ui/Input";
import DarkWrapperBlock from "../../../components/layout/DarkWrapperBlock";
import SaveButton from "../../../components/ui/SaveButton";
import SettingsForm from "../../../components/layout/SettingsForm";
import FormField from "../../../components/ui/FormField";
import SelectToggle from "../../../components/ui/SelectToggle";
import WrapperBlock from "../../../components/layout/WrapperBlock";
import { useBotSettings } from "../hooks/useBotSettings";

const Bot = () => {
  const { botToken, setBotToken, botPassword, setBotPassword, isBotEnabled, setIsBotEnabled, setTelegramBotSettings, setIsAlertNotificationsEnabled, isAlertNotificationsEnabled } = useBotSettings();

  return (
    <SettingsForm title='Телеграм бот'>
      <WrapperBlock>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label='Увімкнений'>
            <SelectToggle
              value={isBotEnabled}
              onChange={(e) => setIsBotEnabled(e.target.value)}
              className='w-full'
            />
          </FormField>
          <FormField label='Токен'>
            <Input className='w-full' value={botToken} onChange={(e) => setBotToken(e.target.value)} />
          </FormField>
          <FormField label='Фраза реєстрації'>
            <Input className='w-full' value={botPassword} onChange={(e) => setBotPassword(e.target.value)} />
          </FormField>
          <FormField label='Сповіщення безпеки'>
            <SelectToggle
              value={isAlertNotificationsEnabled}
              onChange={(e) => setIsAlertNotificationsEnabled(e.target.value)}
              className='w-full'
            />
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>
      <SaveButton onClick={() => setTelegramBotSettings(botToken, botPassword, isBotEnabled, isAlertNotificationsEnabled)} />
    </SettingsForm>
  )
}

export default Bot