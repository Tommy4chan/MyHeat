import { useEffect, useState } from "react";
import useSettingStore from "@/store/settingStore";

export const useBotSettings = () => {
  const [botToken, setBotToken] = useState('');
  const [botPassword, setBotPassword] = useState('');
  const [isBotEnabled, setIsBotEnabled] = useState(false);
  const [isAlertNotificationsEnabled, setIsAlertNotificationsEnabled] = useState(false);

  const { setTelegramBotSettings, getTelegramBotSettings, telegramBotSettings } = useSettingStore();

  useEffect(() => {
    getTelegramBotSettings();
  }, []);

  useEffect(() => {
    setBotToken(telegramBotSettings.token);
    setBotPassword(telegramBotSettings.registerPhrase);
    setIsBotEnabled(telegramBotSettings.isEnabled);
    setIsAlertNotificationsEnabled(telegramBotSettings.isAlertNotificationsEnabled);
  }, [telegramBotSettings]);

  return {
    botToken,
    setBotToken,
    botPassword,
    setBotPassword,
    isBotEnabled,
    setIsBotEnabled,
    setTelegramBotSettings,
    isAlertNotificationsEnabled,
    setIsAlertNotificationsEnabled
  }
}