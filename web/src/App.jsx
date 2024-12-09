import { useEffect, useState } from 'react';
import SettingsPage from './pages/SettingsPage';
import Navbar from './components/Navbar';
import DashboardPage from './pages/DashboardPage';
import TemperaturesPage from './pages/TemperaturesPage';
import RelaysPage from './pages/RelaysPage';
import FunctionsPage from './pages/FunctionsPage';
import usePinStore from './store/pinStore';
import useWebSocketStore from './store/websocketStore';

function App() {
  const [page, setPage] = useState('dashboard');

  const { initializePins } = usePinStore();
  const connect = useWebSocketStore(state => state.connect);

  useEffect(() => {
    initializePins();
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
