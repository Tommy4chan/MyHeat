import { useEffect, useState } from 'react';
import useWebSocketStore from './store/websocketStore';
import Navbar from './components/layout/Navbar';
import { DashboardPage } from './pages/dashboard';
import { TemperaturesPage } from './pages/temperatures';
import { RelaysPage } from './pages/relays';
import { FunctionsPage } from './pages/functions';
import { SettingsPage } from './pages/settings';


function App() {
  const [page, setPage] = useState('dashboard');

  const connect = useWebSocketStore(state => state.connect);

  useEffect(() => {
    connect();
  }, []);

  return (
    <div className="flex flex-col min-h-screen bg-gray-900 text-gray-200">
      <Navbar setPage={setPage} />
      <div className="container mx-auto h-full my-6 pt-[52px] md:pt-0 px-2 md:px-0">
        {page === 'dashboard' && <DashboardPage />}
        {page === 'temperatures' && <TemperaturesPage />}
        {page === 'relays' && <RelaysPage />}
        {page === 'functions' && <FunctionsPage />}
        {page === 'settings' && <SettingsPage />}
      </div>
    </div>
  )
}

export default App;
