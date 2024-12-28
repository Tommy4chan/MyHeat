import Temperatures from '../../components/Temperatures'
import Functions from '../../components/Functions'
import { Relays } from '../../components/Relays'

const DashboardPage = () => {
  return (
    <div className="grid grid-cols-1 xl:grid-cols-3 items-start gap-6">
      <Temperatures />
      <Relays />
      <Functions />
    </div>
  )
}

export default DashboardPage