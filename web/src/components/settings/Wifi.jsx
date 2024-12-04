import React, { useState } from 'react'
import Button from '../ui/buttons/Button'
import DarkWrapperBlock from '../ui/DarkWrapperBlock';
import Input from '../ui/Input';
import FormColumn from '../ui/FormColumn';
import SaveButton from '../ui/buttons/SaveButton';
import FormField from '../ui/FormField';

const Wifi = () => {
  const [ssid, setSsid] = useState('');
  const [password, setPassword] = useState('');
  const [mdns, setMdns] = useState('');

  const [networks, setNetworks] = useState([
    { id: 1, ssid: 'www.matrixtel.net', signal: 80 },
    { id: 2, ssid: 'Network 2', signal: 60 },
    { id: 3, ssid: 'Network 3', signal: 40 },
  ]);

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
      <div className='flex flex-col gap-2 w-full'>
        <h3 className='text-xl'>Доступні мережі:</h3>
        {networks.length === 0 ?
          <DarkWrapperBlock><p>Мереж не знайдено</p></DarkWrapperBlock>
          :
          networks.map((network) => (
            <DarkWrapperBlock key={network.id}>
              <div className='flex justify-between w-full'>
                <p>{network.ssid}</p>
                <p>{network.signal}db</p>
              </div>
              <Button
                buttonText={'Обрати'}
                onClick={() => setSsid(network.ssid)}
              />
            </DarkWrapperBlock>
          ))
        }
        <Button buttonText={'Сканувати'} color='indigo' />
      </div>
      <div className='flex flex-col gap-2 w-full'>
        <h3 className='text-xl'>Мережа:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label="Ім'я">
            <Input className='w-full' value={ssid} onChange={(e) => setSsid(e.target.value)} />
          </FormField>
          <FormField label="Пароль">
            <Input className='w-full' value={password} onChange={(e) => setPassword(e.target.value)} />
          </FormField>
        </DarkWrapperBlock>
      </div>
      <div className='flex flex-col gap-2 w-full'>
        <h3 className='text-xl'>mDns:</h3>
        <DarkWrapperBlock className='md:!flex-col'>
          <FormField label="Адреса контролера">
            <div className='relative flex items-center rounded focus:outline-none focus:ring transition duration-300 bg-gray-700 hover:bg-gray-800 focus:ring-gray-800'>
              <Input className='w-full !bg-transparent focus:!ring-0 !pe-0' value={mdns} onChange={(e) => setMdns(e.target.value)} />
              <span className='pe-2 flex items-center'>.local</span>
            </div>
          </FormField>
        </DarkWrapperBlock>
        <SaveButton />
      </div>
    </FormColumn>
  )
}

export default Wifi