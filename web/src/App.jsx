import { useState } from 'react';
import Navbar from './components/layout/Navbar';
import { DashboardPage } from './pages/dashboard';
import { TemperaturesPage } from './pages/temperatures';
import { RelaysPage } from './pages/relays';
import { FunctionsPage } from './pages/functions';
import { SettingsPage } from './pages/settings';
import { WebSocketContainer } from './components/WebSocketContainer';
import { ToastContainer } from 'react-toastify';
import useWebSocketStore from './store/websocketStore';

function App() {
  const [page, setPage] = useState('dashboard');

  return (
    <>
      <WebSocketContainer />
      <div className="flex flex-col min-h-screen bg-gray-900 text-gray-200">
        <Navbar setPage={setPage} page={page} />
        <div className="container mx-auto h-full my-6 pt-[52px] md:pt-0 px-2 md:px-0">
          {page === 'dashboard' && <DashboardPage />}
          {page === 'temperatures' && <TemperaturesPage />}
          {page === 'relays' && <RelaysPage />}
          {page === 'functions' && <FunctionsPage />}
          {page === 'settings' && <SettingsPage />}
        </div>
      </div>
      <ToastContainer/>
    </>
  )
}

export default App;
