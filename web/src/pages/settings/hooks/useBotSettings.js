import { useEffect, useState } from "react";
import useSettingStore from "@/store/settingStore";

export const useBotSettings = () => {
  const [botToken, setBotToken] = useState('');
  const [botPassword, setBotPassword] = useState('');
  const [isBotEnabled, setIsBotEnabled] = useState(false);
  const [isAlertNotificationsEnabled, setIsAlertNotificationsEnabled] = useState(false);
  const [isBootNotificationEnabled, setIsBootNotificationEnabled] = useState(false);
  const [users, setUsers] = useState([]);

  const { setTelegramBotSettings, getTelegramBotSettings, telegramBotSettings, deleteTelegramUser } = useSettingStore();

  useEffect(() => {
    getTelegramBotSettings();
  }, []);

  useEffect(() => {
    setBotToken(telegramBotSettings.token);
    setBotPassword(telegramBotSettings.registerPhrase);
    setIsBotEnabled(telegramBotSettings.isEnabled);
    setIsAlertNotificationsEnabled(telegramBotSettings.isAlertNotificationsEnabled);
    setIsBootNotificationEnabled(telegramBotSettings.isBootNotificationEnabled ?? false);
    setUsers(telegramBotSettings.users || []);
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
    setIsAlertNotificationsEnabled,
    isBootNotificationEnabled,
    setIsBootNotificationEnabled,
    users,
    deleteTelegramUser
  }
}