import React from 'react'
import { RiSignalWifi1Fill, RiSignalWifi2Fill, RiSignalWifi3Fill, RiSignalWifiFill } from "react-icons/ri";

const WifiSignal = ({ signal }) => {
  if (signal >= -50) {
    return <RiSignalWifiFill size={22} />
  } else if (signal >= -65) {
    return <RiSignalWifi3Fill size={22} />
  } else if (signal >= -75) {
    return <RiSignalWifi2Fill size={22} />
  } else {
    return <RiSignalWifi1Fill size={22} />
  }

}

export default WifiSignal