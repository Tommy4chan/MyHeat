import { useState } from 'react';
import SettingsPage from './pages/SettingsPage';
import Navbar from './components/Navbar';
import DashboardPage from './pages/DashboardPage';
import Wifi from './pages/Wifi';
import TemperaturesPage from './pages/TemperaturesPage';
import RelaysPage from './pages/RelaysPage';
import FunctionsPage from './pages/FunctionsPage';

function App() {
  const [page, setPage] = useState('dashboard');

  return (
    <div className="flex flex-col h-screen bg-gray-900 text-gray-200">
      <Navbar setPage={setPage}/>
      <div className="container mx-auto h-full mt-6">
        {page === 'dashboard' && <DashboardPage />}
        {page === 'temperatures' && <TemperaturesPage />}
        {page === 'relays' && <RelaysPage />}
        {page === 'functions' && <FunctionsPage />}
        {page === 'wifi' && <Wifi />}
        {page === 'settings' && <SettingsPage />}
      </div>
  </div>
  )
}

export default App;
