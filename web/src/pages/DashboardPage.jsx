import React from 'react'
import Temperatures from '../components/Temperatures'
import Relays from '../components/Relays'
import Functions from '../components/Functions'

const DashboardPage = () => {
  return (
    <div className="flex items-start gap-6">
      <Temperatures />
      <Relays />
      <Functions />
    </div>
  )
}

export default DashboardPage