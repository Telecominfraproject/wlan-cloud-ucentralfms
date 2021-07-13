//
// Created by stephane bourque on 2021-07-13.
//

#include "LatestFirmwareCache.h"
#include "StorageService.h"

namespace uCentral {
    class LatestFirmwareCache *LatestFirmwareCache::instance_ = nullptr;

    int LatestFirmwareCache::Start() {
        Storage()->PopulateLatestFirmwareCache();
        return 0;
    }

    void LatestFirmwareCache::Stop() {
    }

    void LatestFirmwareCache::AddToCache(const std::string & DeviceType, const std::string &Id, uint64_t TimeStamp) {
        SubMutexGuard G(Mutex_);

        auto E = FirmwareCache_.find(DeviceType);
        if((E==FirmwareCache_.end()) || (TimeStamp < E->second.TimeStamp)) {
            FirmwareCache_[DeviceType] = FirmwareCacheEntry{.Id=Id, .TimeStamp=TimeStamp};
        }
    }

    std::string LatestFirmwareCache::FindLatestFirmware(std::string &DeviceType)  {
        SubMutexGuard G(Mutex_);

        std::string Result;
        auto E=FirmwareCache_.find(DeviceType);
        if(E!=FirmwareCache_.end())
            Result = E->second.Id;
        return Result;
    }

    void LatestFirmwareCache::DumpCache() {
        SubMutexGuard G(Mutex_);

        for( auto &[Id,E]:FirmwareCache_) {
            std::cout << "Device: " << Id << "    ID:" << E.Id << std::endl;
        }

    }
}