//
// Created by stephane bourque on 2021-07-13.
//

#ifndef UCENTRALFMS_DEVICECACHE_H
#define UCENTRALFMS_DEVICECACHE_H

#include <string>
#include "SubSystemServer.h"
#include "uCentralTypes.h"

namespace uCentral {

    struct DeviceCacheEntry {
        std::string     compatible;
        std::string     host;
        std::string     firmware;
    };
    typedef std::map<std::string, DeviceCacheEntry> DeviceCacheMap;

    class DeviceCache : public SubSystemServer {
    public:
        static DeviceCache *instance() {
            if (instance_ == nullptr) {
                instance_ = new DeviceCache;
            }
            return instance_;
        }

        int Start() override;
        void Stop() override;
        void AddToCache(const std::string &serialNumber, const std::string & DeviceType,
                        const std::string &Host, const std::string &Firmware);
        std::string FindLatestFirmware(std::string &DeviceType);
        void DumpCache();
        bool GetDevice(const std::string &SerialNumber, DeviceCacheEntry & E);

    private:
        static DeviceCache 	*instance_;
        std::atomic_bool    Running_=false;
        DeviceCacheMap      DeviceCache_;
        explicit DeviceCache() noexcept:
                SubSystemServer("DeviceCache", "DEVICE-CACHE", "devicecache")
        {
        }
    };

    inline DeviceCache * DeviceCache() { return DeviceCache::instance(); }
}



#endif //UCENTRALFMS_DEVICECACHE_H
