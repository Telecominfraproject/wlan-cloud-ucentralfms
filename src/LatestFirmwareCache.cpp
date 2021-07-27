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

    void LatestFirmwareCache::AddToCache(const std::string & DeviceType, const std::string &Revision, const std::string &Id, uint64_t TimeStamp) {
        SubMutexGuard G(Mutex_);

        RevisionSet_.insert(Revision);
        DeviceSet_.insert(DeviceType);
        auto E = Cache_.find(DeviceType);
        if((E==Cache_.end()) || (TimeStamp > E->second.TimeStamp)) {
            Cache_[DeviceType] = LatestFirmwareCacheEntry{.Id=Id,
                                                            .TimeStamp=TimeStamp,
                                                            .Revision=Revision};
        }
    }

    bool LatestFirmwareCache::FindLatestFirmware(const std::string &DeviceType, LatestFirmwareCacheEntry &Entry )  {
        SubMutexGuard G(Mutex_);

        auto E=Cache_.find(DeviceType);
        if(E!=Cache_.end()) {
            Entry = E->second;
            return true;
        }

        return false;
    }

    bool LatestFirmwareCache::IsLatest(const std::string &DeviceType, const std::string &Revision) {
        SubMutexGuard G(Mutex_);

        auto E=Cache_.find(DeviceType);
        if(E!=Cache_.end()) {
            return E->second.Revision==Revision;
        }
        return false;
    }


    void LatestFirmwareCache::DumpCache() {
        SubMutexGuard G(Mutex_);

        for( auto &[Id,E]:Cache_) {
            std::cout << "Device: " << Id << "    ID:" << E.Id << std::endl;
        }

    }
}