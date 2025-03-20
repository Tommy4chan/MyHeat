import React, { useEffect, useState } from 'react'
import Button from '../../../components/ui/Button'
import DarkWrapperBlock from '../../../components/layout/DarkWrapperBlock';
import Input from '../../../components/ui/Input';
import SettingsForm from '../../../components/layout/SettingsForm';
import SaveButton from '../../../components/ui/SaveButton';
import FormField from '../../../components/ui/FormField';
import WrapperBlock from '../../../components/layout/WrapperBlock';
import useSettingStore from '../../../store/settingStore';
import WifiSignal from '../../../components/ui/WifiSignal';

const Wifi = () => {
  const [wifiSSID, setWifiSSID] = useState('');
  const [wifiPassword, setWifiPassword] = useState('');

  const [apSSID, setApSSID] = useState('');
  const [apPassword, setApPassword] = useState('');

  const [mDNS, setMDNS] = useState('');
  const [networks, setNetworks] = useState([]);

  const { setWifiSettings, getWifiSettings, startWifiScan, wifiSettings, scannedWifiNetworks, isScanningForWifiNetworks } = useSettingStore();

  useEffect(() => {
    getWifiSettings();
  }, []);

  useEffect(() => {
    setWifiSSID(wifiSettings.wifiSSID);
    setWifiPassword(wifiSettings.wifiPassword);
    setApSSID(wifiSettings.apSSID);
    setApPassword(wifiSettings.apPassword);
    setMDNS(wifiSettings.mDNS);
  }, [wifiSettings]);

  useEffect(() => {
    setNetworks(scannedWifiNetworks);
  }, [scannedWifiNetworks]);

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
            <Input className='w-full' value={wifiSSID} onChange={(e) => setWifiSSID(e.target.value)} />
          </FormField>
          <FormField label="Пароль">
            <Input className='w-full' value={wifiPassword} onChange={(e) => setWifiPassword(e.target.value)} />
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>Точка доступу:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label="Ім'я">
            <Input className='w-full' value={apSSID} onChange={(e) => setApSSID(e.target.value)} />
          </FormField>
          <FormField label="Пароль">
            <Input className='w-full' value={apPassword} onChange={(e) => setApPassword(e.target.value)} />
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>mDns:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label="Адреса контролера">
            <div className='relative flex items-center rounded focus:outline-none focus:ring transition duration-300 bg-gray-700 hover:bg-gray-800 focus:ring-gray-800'>
              <Input className='w-full !bg-transparent focus:!ring-0 !pe-0' value={mDNS} onChange={(e) => setMDNS(e.target.value)} />
              <span className='pe-2 flex items-center'>.local</span>
            </div>
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>
      <SaveButton onClick={() => setWifiSettings(wifiSSID, wifiPassword, apSSID, apPassword, mDNS)} />
    </SettingsForm>
  )
}

export default Wifi