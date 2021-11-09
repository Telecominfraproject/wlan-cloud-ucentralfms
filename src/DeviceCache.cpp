//
// Created by stephane bourque on 2021-07-13.
//

#include "DeviceCache.h"

namespace OpenWifi {

    int DeviceCache::Start() {
        return 0;
    }

    void DeviceCache::Stop() {
    }

    void DeviceCache::AddToCache(
                        const std::string &SerialNumber, const std::string & DeviceType,
                        const std::string &Host, const std::string &Revision) {
        std::lock_guard G(Mutex_);
        auto Device = DeviceCache_.find(SerialNumber);

        if(Device==DeviceCache_.end()) {
            DeviceCache_[SerialNumber]=DeviceCacheEntry{
                                                .deviceType=DeviceType,
                                                .host=Host,
                                                .revision=Revision};
        } else {
            Device->second.revision=Revision;
            Device->second.host=Host;
            Device->second.deviceType=DeviceType;
        }
    }

    bool DeviceCache::GetDevice(const std::string &SerialNumber, DeviceCacheEntry & E) {
        std::lock_guard G(Mutex_);
        auto Device = DeviceCache_.find(SerialNumber);
        if(Device==DeviceCache_.end())
            return false;
        E=Device->second;
        return true;
    }


    void DeviceCache::DumpCache() {

    }
}