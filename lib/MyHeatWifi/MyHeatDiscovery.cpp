#include "MyHeatDiscovery.h"
#include <ESPmDNS.h>
#include <WiFi.h>
#include <atomic>

namespace MyHeatDiscovery
{
    static const char *SERVICE = "myheat";
    static const char *PROTO = "tcp";

    static std::atomic<bool> isScanRunning{false};
    static std::atomic<bool> isScanDone{false};
    // Written only by the scan task before isScanDone is set, read only after it
    static std::vector<Device> scanResults;

    void advertise(const String &name)
    {
        MDNS.addService("http", PROTO, 80);
        MDNS.addService(SERVICE, PROTO, 80);
        MDNS.addServiceTxt(SERVICE, PROTO, "name", name.c_str());
    }

    static void scanTask(void *)
    {
        std::vector<Device> found;
        int count = MDNS.queryService(SERVICE, PROTO);

        // Our own controller answers too, it is skipped by address
        IPAddress ownAddresses[] = {WiFi.localIP(), WiFi.softAPIP()};

        for (int i = 0; i < count; i++)
        {
            IPAddress address = MDNS.address(i);
            if (address == IPAddress() || address == ownAddresses[0] || address == ownAddresses[1])
                continue;

            String addressText = address.toString();
            bool isDuplicate = false;
            for (const auto &device : found)
            {
                if (device.address == addressText)
                {
                    isDuplicate = true;
                    break;
                }
            }
            if (isDuplicate)
                continue;

            String hostname = MDNS.hostname(i);
            if (hostname.endsWith(".local"))
                hostname.remove(hostname.length() - 6);

            String name = MDNS.txt(i, "name");
            if (name.isEmpty())
                name = hostname;

            found.push_back({name, hostname, addressText});
        }

        scanResults = std::move(found);
        isScanDone.store(true, std::memory_order_release);
        isScanRunning.store(false, std::memory_order_release);
        vTaskDelete(NULL);
    }

    bool startScan()
    {
        bool expected = false;
        if (!isScanRunning.compare_exchange_strong(expected, true))
            return false;

        isScanDone.store(false, std::memory_order_release);
        if (xTaskCreate(scanTask, "mdnsScan", 4096, nullptr, 1, nullptr) != pdPASS)
        {
            isScanRunning.store(false, std::memory_order_release);
            return false;
        }
        return true;
    }

    bool takeResults(std::vector<Device> &devices)
    {
        if (!isScanDone.load(std::memory_order_acquire))
            return false;

        devices = std::move(scanResults);
        scanResults.clear();
        isScanDone.store(false, std::memory_order_release);
        return true;
    }
}
