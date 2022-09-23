//
// Created by stephane bourque on 2021-07-13.
//

#include "LatestFirmwareCache.h"
#include "StorageService.h"

namespace OpenWifi {

    int LatestFirmwareCache::Start() {
        poco_information(Logger(),"Starting...");
        StorageService()->FirmwaresDB().PopulateLatestFirmwareCache();
        return 0;
    }

    void LatestFirmwareCache::Stop() {
        poco_information(Logger(),"Stopping...");
        poco_information(Logger(),"Stopped...");
    }

    bool LatestFirmwareCache::AddToCache(const std::string & DeviceType, const std::string &Revision, const std::string &Id, uint64_t TimeStamp) {
        std::lock_guard G(Mutex_);

        RevisionSet_.insert(Revision);
        DeviceSet_.insert(DeviceType);

        auto E = Cache_.find(DeviceType);
        if((E==Cache_.end()) || (TimeStamp >= E->second.TimeStamp)) {
            Cache_[DeviceType] = LatestFirmwareCacheEntry{
                .Id=Id,
                .TimeStamp=TimeStamp,
                .Revision=Revision};
        }

        if(!IsRC(Revision))
            return true;

        auto rcE = rcCache_.find(DeviceType);
        if((rcE==rcCache_.end()) || (TimeStamp >= rcE->second.TimeStamp)) {
            rcCache_[DeviceType] = LatestFirmwareCacheEntry{
                .Id=Id,
                .TimeStamp=TimeStamp,
                .Revision=Revision};
        }
        return true;
    }

    bool LatestFirmwareCache::FindLatestFirmware(const std::string &DeviceType, LatestFirmwareCacheEntry &Entry )  {
        std::lock_guard G(Mutex_);

        auto E=Cache_.find(DeviceType);
        if(E!=Cache_.end()) {
            Entry = E->second;
            return true;
        }
        return false;
    }

    bool LatestFirmwareCache::FindLatestRCOnlyFirmware(const std::string &DeviceType, LatestFirmwareCacheEntry &Entry ) {
        std::lock_guard G(Mutex_);

        auto E=rcCache_.find(DeviceType);
        if(E!=rcCache_.end()) {
            Entry = E->second;
            return true;
        }
        return false;
    }


    bool LatestFirmwareCache::IsLatest(const std::string &DeviceType, const std::string &Revision) {
        std::lock_guard G(Mutex_);

        auto E=Cache_.find(DeviceType);
        if(E!=Cache_.end()) {
            return E->second.Revision==Revision;
        }
        return false;
    }

    bool LatestFirmwareCache::IsLatestRCOnly(const std::string &DeviceType, const std::string &Revision) {
        std::lock_guard G(Mutex_);

        auto E=rcCache_.find(DeviceType);
        if(E!=rcCache_.end()) {
            return E->second.Revision==Revision;
        }
        return false;
    }


    void LatestFirmwareCache::DumpCache() {
        std::lock_guard G(Mutex_);

        for( auto &[Id,E]:Cache_) {
            std::cout << "Device: " << Id << "    ID:" << E.Id << std::endl;
        }

    }
}