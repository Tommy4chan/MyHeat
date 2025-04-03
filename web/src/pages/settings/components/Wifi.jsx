import Button from '@/components/ui/Button'
import DarkWrapperBlock from '@/components/layout/DarkWrapperBlock';
import Input from '@/components/ui/Input';
import SettingsForm from '@/components/layout/SettingsForm';
import SaveButton from '@/components/ui/SaveButton';
import FormField from '@/components/ui/FormField';
import WrapperBlock from '@/components/layout/WrapperBlock';
import WifiSignal from '@/components/ui/WifiSignal';
import SelectToggle from "@/components/ui/SelectToggle";
import { useWifiSettings } from '../hooks/useWifiSettings';

const Wifi = () => {
  const {
    settings,
    networks,
    handleSettingsChange,
    handleWifiSettingsSave,
    startWifiScan,
    isScanningForWifiNetworks,
    setWifiSSID
  } = useWifiSettings();

  return (
    <SettingsForm title='Wifi'>
      <WrapperBlock>
        <h3 className='text-xl'>Доступні мережі:</h3>
        {isScanningForWifiNetworks ? <DarkWrapperBlock><p>Сканування...</p></DarkWrapperBlock> :
          networks.length === 0 ?
            <DarkWrapperBlock><p>Мереж не знайдено</p></DarkWrapperBlock>
            :
            networks.map((network) => (
              <DarkWrapperBlock key={network.id}>
                <div className='flex justify-between w-full'>
                  <p>{network.ssid}</p>
                  <div className="flex gap-1 items-center">
                    <p>{network.rssi}dBm</p>
                    <WifiSignal signal={network.rssi} />
                  </div>

                </div>
                <Button
                  buttonText={'Обрати'}
                  onClick={() => setWifiSSID(network.ssid)}
                />
              </DarkWrapperBlock>
            ))
        }
        <Button buttonText={'Сканувати'} color='indigo' onClick={startWifiScan} />
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>Мережа:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label="Ім'я">
            <Input className='w-full' value={settings.wifiSSID} onChange={handleSettingsChange} name='wifiSSID' />
          </FormField>
          <FormField label="Пароль">
            <Input className='w-full' value={settings.wifiPassword} onChange={handleSettingsChange} name='wifiPassword' />
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>Точка доступу:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label="Ім'я">
            <Input className='w-full' value={settings.apSSID} onChange={handleSettingsChange} name='apSSID' />
          </FormField>
          <FormField label="Пароль">
            <Input className='w-full' value={settings.apPassword} onChange={handleSettingsChange} name='apPassword' />
          </FormField>
          <FormField label="Резервна точка доступу">
            <SelectToggle
              value={settings.isFallbackAPEnabled}
              onChange={handleSettingsChange}
              className='w-full'
              name='isFallbackAPEnabled'
            />
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>mDns:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label="Адреса контролера">
            <div className='relative flex items-center rounded focus:outline-none focus:ring transition duration-300 bg-gray-700 hover:bg-gray-800 focus:ring-gray-800'>
              <Input className='w-full !bg-transparent focus:!ring-0 !pe-0' value={settings.mDNS} onChange={handleSettingsChange} name='mDNS' />
              <span className='pe-2 flex items-center'>.local</span>
            </div>
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>
      <SaveButton onClick={handleWifiSettingsSave} />
    </SettingsForm>
  )
}

export default Wifi