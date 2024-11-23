const Navbar = ({ setPage }) => {
  return (
    <nav className=" bg-gray-800">
      <div className="flex justify-between items-center container mx-auto">
        <div className="flex gap-3">
          <img className="w-8" src="/logo-transparent.png"/>
          <h2 className="text-2xl font-semibold">MyHeat</h2>
        </div>
        <div>
          <ul className="flex">
            <li><button className="hover:bg-gray-900 transition duration-300 p-4" onClick={() => setPage('dashboard')}>Дешборд</button></li>
            <li><button className="hover:bg-gray-900 transition duration-300 p-4" onClick={() => setPage('temperatures')}>Температура</button></li>
            <li><button className="hover:bg-gray-900 transition duration-300 p-4" onClick={() => setPage('relays')}>Реле</button></li>
            <li><button className="hover:bg-gray-900 transition duration-300 p-4" onClick={() => setPage('functions')}>Функції</button></li>
            <li><button className="hover:bg-gray-900 transition duration-300 p-4" onClick={() => setPage('wifi')}>Налаштування</button></li>
          </ul>
        </div>
      </div>
    </nav>
  )
}

export default Navbar