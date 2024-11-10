const Navbar = ({ setPage }) => {
  return (
    <nav className="p-4 bg-gray-800">
      <div className="flex justify-between items-center container mx-auto">
        <div className="flex gap-3">
          <img className="w-8" src="/logo-transparent.png"/>
          <h2 className="text-2xl font-semibold">MyHeat</h2>
        </div>
        <div>
          <ul className="flex space-x-4">
            <li><button className="hover:text-gray-400" onClick={() => setPage('dashboard')}>Dashboard</button></li>
            <li><button className="hover:text-gray-400" onClick={() => setPage('wifi')}>Wifi</button></li>
            <li><button className="hover:text-gray-400" onClick={() => setPage('settings')}>Settings</button></li>
            <li><button className="hover:text-gray-400" onClick={() => setPage('')}>Logout</button></li>
          </ul>
        </div>
      </div>
    </nav>
  )
}

export default Navbar