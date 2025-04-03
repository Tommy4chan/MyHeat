import { useState } from "react";
import { AiOutlineClose, AiOutlineMenu } from 'react-icons/ai';
import WebSocketStatus from "./WebSocketStatus";

const Navbar = ({ setPage, page }) => {
  const [nav, setNav] = useState(false);

  const handleNav = () => {
    setNav(!nav);
  };

  const navOptions = [
    {
      name: 'Дешборд',
      page: 'dashboard',
    },
    {
      name: 'Температура',
      page: 'temperatures',
    },
    {
      name: 'Реле',
      page: 'relays',
    },
    {
      name: 'Функції',
      page: 'functions',
    },
    {
      name: 'Налаштування',
      page: 'settings',
    }
  ];

  return (
    <nav className="fixed md:relative w-full bg-gray-800">
      <div className="flex justify-between items-center container mx-auto">
        <div className="flex">
          <div className="flex gap-2 mx-4 items-center">
            <img className="w-8 h-8" src="/logo-transparent.png" />
            <h2 className="text-2xl font-semibold">MyHeat</h2>
          </div>
            <WebSocketStatus className="hidden lg:flex" />
        </div>

        <div>
          <ul className="flex">
            {navOptions.map((option) => (
              <li className={`hidden lg:block hover:bg-gray-900 transition duration-300 ${option.page == page && 'bg-gray-900'}`} key={option.name}>
                <button className="p-4" onClick={() => setPage(option.page)}>{option.name}</button>
              </li>
            ))}
            <li className="block lg:hidden hover:bg-gray-900 transition duration-300">
              <button className="p-4" onClick={handleNav}>{nav ? <AiOutlineClose size={20} /> : <AiOutlineMenu size={20} />}</button>
            </li>
          </ul>
        </div>
      </div>
      <div className="lg:hidden">
        <ul
          className={`absolute w-full bg-gray-800 overflow-hidden transition-all duration-300 ease-in-out ${nav ? 'max-h-96 opacity-100' : 'max-h-0 opacity-0'
            }`}
        >
          <li className="w-full flex justify-center items-center py-4 border-b border-gray-700">
          <WebSocketStatus />
          </li>
          
          {navOptions.map((option) => (
            <li
              key={option.name}
              className={`hover:bg-gray-900 transition duration-300 ${option.page == page && 'bg-gray-900'}`}
            >
              <button
                className="py-4 w-full"
                onClick={() => {
                  setPage(option.page);
                  setNav(false);
                }}
              >
                {option.name}
              </button>
            </li>
          ))}
        </ul>
      </div>
    </nav>
  )
}

export default Navbar