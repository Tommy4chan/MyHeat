import React from 'react'

const Dashboard = () => {
  return (
    <main className="flex-1">
      <h1 className="text-3xl font-semibold mb-6">Welcome to MyHeat</h1>

      {/* Cards Section */}
      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-6">
        {/* Card 1: Current Temperature Sensor */}
        <div className="bg-gray-800 border border-gray-700 rounded-lg shadow-md p-4">
          <h2 className="font-semibold text-lg">Temperature Sensor 1</h2>
          <p className="text-2xl text-gray-300">72°F</p>
        </div>

        {/* Card 2: Temperature Sensor 2 */}
        <div className="bg-gray-800 border border-gray-700 rounded-lg shadow-md p-4">
          <h2 className="font-semibold text-lg">Temperature Sensor 2</h2>
          <p className="text-2xl text-gray-300">70°F</p>
        </div>

        {/* Card 3: Boiler Status */}
        <div className="bg-gray-800 border border-gray-700 rounded-lg shadow-md p-4">
          <h2 className="font-semibold text-lg">Boiler Status</h2>
          <p className="text-2xl text-green-500">Running</p>
        </div>

        {/* Card 4: Energy Usage */}
        <div className="bg-gray-800 border border-gray-700 rounded-lg shadow-md p-4">
          <h2 className="font-semibold text-lg">Energy Usage</h2>
          <p className="text-2xl text-yellow-400">350 kWh</p>
        </div>

        {/* Card 5: Relay 1 Status */}
        <div className="bg-gray-800 border border-gray-700 rounded-lg shadow-md p-4">
          <h2 className="font-semibold text-lg">Relay 1 Status</h2>
          <p className="text-2xl text-gray-300">Active</p>
        </div>

        {/* Card 6: Relay 2 Status */}
        <div className="bg-gray-800 border border-gray-700 rounded-lg shadow-md p-4">
          <h2 className="font-semibold text-lg">Relay 2 Status</h2>
          <p className="text-2xl text-gray-300">Inactive</p>
        </div>

        {/* Card 7: Alerts */}
        <div className="bg-gray-800 border border-gray-700 rounded-lg shadow-md p-4">
          <h2 className="font-semibold text-lg">Alerts</h2>
          <p className="text-2xl text-red-500">No Alerts</p>
        </div>
      </div>
    </main>
  )
}

export default Dashboard