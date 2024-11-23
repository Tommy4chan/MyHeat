import { useState } from "react";

const Wifi = () => {
  const [ssid, setSsid] = useState('');
  const [password, setPassword] = useState('');

  const [networks, setNetworks] = useState([
    { id: 1, ssid: 'www.matrixtel.net', signal: 80 },
    { id: 2, ssid: 'Network 2', signal: 60 },
    { id: 3, ssid: 'Network 3', signal: 40 },
  ]);

  const handleSubmit = (e) => {
    e.preventDefault();

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
    <div className="flex justify-center items-center w-full h-full">
      <div className="bg-gray-800 border border-gray-700 rounded-lg shadow-md p-4 w-96">
        <h2 className="text-xl text-center font-semibold">WiFi Мережі</h2>
        <div className='flex flex-col gap-3 my-3 w-full'>
          {networks.length === 0 ? <div>No networks found</div>
            :
            networks.map((network) => (
              <div key={network.id} className='flex justify-between items-center'>
                <div className="flex justify-between w-full me-3">
                  <span>{network.ssid}</span>
                  <span>{network.signal}db</span>
                </div>
                <button className='px-3 py-2 text-xs font-medium text-center text-white  focus:outline-none bg-gray-600 hover:bg-gray-700 focus:ring-gray-900 rounded-lg' onClick={() => setSsid(network.ssid)}>Обрати</button>
              </div>
            ))
          }
        </div>
        <div className="flex justify-center">
          <button className="focus:outline-none text-white bg-purple-600 hover:bg-purple-700 focus:ring-purple-900 font-medium rounded-lg text-sm px-5 py-2.5 my-2">Сканувати</button>
        </div>

        <h2 className="text-xl text-center font-semibold my-3">WiFi Configuration</h2>
        <form onSubmit={handleSubmit}>
          <div className='flex flex-col gap-3 mb-3'>
            <div>
              <label for="ssid" class="block mb-2 text-sm font-medium text-white">Ім'я мережі</label>
              <input type="text" id="ssid" class="borde text-sm rounded-lg block w-full p-2.5 bg-gray-700 border-gray-600 placeholder-gray-400 text-white focus:ring-blue-500 focus:border-blue-500" placeholder="" onChange={(e) => setSsid(e.target.value)} required />
            </div>
            <div>
              <label for="password" class="block mb-2 text-sm font-medium text-white">Пароль</label>
              <input type="password" id="password" class="borde text-sm rounded-lg block w-full p-2.5 bg-gray-700 border-gray-600 placeholder-gray-400 text-white focus:ring-blue-500 focus:border-blue-500" placeholder="" onChange={(e) => setPassword(e.target.value)} required />
            </div>
          </div>
          <div className="flex justify-center">
            <button type="submit" className="focus:outline-none text-white bg-blue-600 hover:bg-blue-700 focus:ring-blue-900 font-medium rounded-lg text-sm px-5 py-2.5 my-2">Зберегти</button>
          </div>
        </form>
      </div>
    </div>
  );
}

export default Wifi