import { useState } from "react";
import { AiOutlineClose, AiOutlineMenu } from 'react-icons/ai';

const Navbar = ({ setPage }) => {
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
        <div className="flex gap-3 mx-4">
          <img className="w-8" src="/logo-transparent.png" />
          <h2 className="text-2xl font-semibold">MyHeat</h2>
        </div>
        <div>
          <ul className="flex">
            {navOptions.map((option) => (
              <li className="hidden md:block hover:bg-gray-900 transition duration-300" key={option.name}>
                <button className="p-4" onClick={() => setPage(option.page)}>{option.name}</button>
              </li>
            ))}

            <li className="block md:hidden hover:bg-gray-900 transition duration-300">
              <button className="p-4" onClick={handleNav}>{nav ? <AiOutlineClose size={20} /> : <AiOutlineMenu size={20} />}</button>
            </li>
          </ul>
        </div>
      </div>
      <div className="md:hidden">
        <ul
          className={`absolute w-full bg-gray-800 overflow-hidden transition-all duration-300 ease-in-out ${
            nav ? 'max-h-96 opacity-100' : 'max-h-0 opacity-0'
          }`}
        >
          {navOptions.map((option) => (
            <li
              key={option.name}
              className="hover:bg-gray-900 transition duration-300"
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