//
// Created by stephane bourque on 2021-07-13.
//

#include "DeviceCache.h"

namespace uCentral {
    class DeviceCache *DeviceCache::instance_ = nullptr;

    int DeviceCache::Start() {
        return 0;
    }

    void DeviceCache::Stop() {
    }

    void DeviceCache::AddToCache(
                        const std::string &SerialNumber, const std::string & DeviceType,
                        const std::string &Host, const std::string &Firmware) {
        SubMutexGuard G(Mutex_);
        auto Device = DeviceCache_.find(SerialNumber);

        if(Device==DeviceCache_.end()) {
            DeviceCache_[SerialNumber]=DeviceCacheEntry{
                                                .compatible=DeviceType,
                                                .host=Host,
                                                .firmware=Firmware};
        } else {
            Device->second.firmware=Firmware;
            Device->second.host=Host;
            Device->second.compatible=DeviceType;
        }
    }

    bool DeviceCache::GetDevice(const std::string &SerialNumber, DeviceCacheEntry & E) {
        SubMutexGuard G(Mutex_);
        auto Device = DeviceCache_.find(SerialNumber);
        if(Device==DeviceCache_.end())
            return false;
        E=Device->second;
        return true;
    }


    void DeviceCache::DumpCache() {

    }
}