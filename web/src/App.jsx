import { useState } from 'react';
import Settings from './components/Settings';
import Navbar from './components/Navbar';
import Dashboard from './components/Dashboard';
import Wifi from './components/Wifi';

function App() {
  const [page, setPage] = useState('dashboard');

  return (
    <div className="flex flex-col h-screen bg-gray-900 text-gray-200">
      <Navbar setPage={setPage}/>
      <div className="container mx-auto p-6 h-full">
        {page === 'dashboard' && <Dashboard />}
        {page === 'wifi' && <Wifi />}
        {page === 'settings' && <Settings />}
      </div>


  </div>
  )
}

export default App;
