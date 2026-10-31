#ifndef MYHEATDISCOVERY_H
#define MYHEATDISCOVERY_H

#include <Arduino.h>
#include <vector>

// Finds other MyHeat controllers in the network through mDNS (_myheat._tcp)
// A query blocks for ~3s, so it only runs on request and in its own task to keep the main loop free
namespace MyHeatDiscovery
{
    struct Device
    {
        String name;
        String hostname; // mDNS name without .local
        String address;
    };

    // Registers this controller so the others can find it, called after every MDNS.begin()
    void advertise(const String &name);

    // Returns false if a scan is already running
    bool startScan();
    // True once per finished scan, the results are moved into devices
    bool takeResults(std::vector<Device> &devices);
}

#endif
