import React, { useEffect, useState } from 'react'
import Button from '../../../components/ui/Button'
import DarkWrapperBlock from '../../../components/layout/DarkWrapperBlock';
import Input from '../../../components/ui/Input';
import FormColumn from '../../../components/layout/FormColumn';
import SaveButton from '../../../components/ui/SaveButton';
import FormField from '../../../components/ui/FormField';
import WrapperBlock from '../../../components/layout/WrapperBlock';
import useSettingStore from '../../../store/settingStore';

const Wifi = () => {
  const [ssid, setSsid] = useState('');
  const [password, setPassword] = useState('');

  const { setWifiCredentials, getWifiSettings, startWifiScan, wifiSettings, scannedWifiNetworks } = useSettingStore();

  useEffect(() => {
    getWifiSettings();
  }, []);

  useEffect(() => {
    setSsid(wifiSettings.ssid);
    setPassword(wifiSettings.password);
  }, [wifiSettings]);

  const [mdns, setMdns] = useState('');

  const [networks, setNetworks] = useState([]);

  useEffect(() => {
    setNetworks(scannedWifiNetworks);
  }, [scannedWifiNetworks]);

  const handleSubmit = () => {
    const request = async () => {
      const requestOptions = {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ ssid, password })
      };

      const response = await fetch('/api/wifi/set', requestOptions);
      const data = await response.json();
      console.log(data);
    }

    request();
  };

  return (
    <FormColumn title='Wifi'>
      <WrapperBlock>
        <h3 className='text-xl'>Доступні мережі:</h3>
        {networks.length === 0 ?
          <DarkWrapperBlock><p>Мереж не знайдено</p></DarkWrapperBlock>
          :
          networks.map((network) => (
            <DarkWrapperBlock key={network.id}>
              <div className='flex justify-between w-full'>
                <p>{network.ssid}</p>
                <p>{network.rssi}dBm</p>
              </div>
              <Button
                buttonText={'Обрати'}
                onClick={() => setSsid(network.ssid)}
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
            <Input className='w-full' value={ssid} onChange={(e) => setSsid(e.target.value)} />
          </FormField>
          <FormField label="Пароль">
            <Input className='w-full' value={password} onChange={(e) => setPassword(e.target.value)} />
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>
      <WrapperBlock>
        <h3 className='text-xl'>mDns:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label="Адреса контролера">
            <div className='relative flex items-center rounded focus:outline-none focus:ring transition duration-300 bg-gray-700 hover:bg-gray-800 focus:ring-gray-800'>
              <Input className='w-full !bg-transparent focus:!ring-0 !pe-0' value={mdns} onChange={(e) => setMdns(e.target.value)} />
              <span className='pe-2 flex items-center'>.local</span>
            </div>
          </FormField>
        </DarkWrapperBlock>
      </WrapperBlock>
      <SaveButton onClick={() => setWifiCredentials(ssid, password)}/>
    </FormColumn>
  )
}

export default Wifi