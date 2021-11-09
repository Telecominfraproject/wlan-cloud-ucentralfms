//
// Created by stephane bourque on 2021-07-13.
//

#ifndef UCENTRALFMS_LATESTFIRMWARECACHE_H
#define UCENTRALFMS_LATESTFIRMWARECACHE_H

#include "Poco/JSON/Object.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/JWT/Signer.h"
#include "Poco/SHA2Engine.h"
#include "RESTObjects/RESTAPI_SecurityObjects.h"
#include "framework/MicroService.h"

namespace OpenWifi {

    struct LatestFirmwareCacheEntry {
        std::string     Id;
        uint64_t        TimeStamp=0;
        std::string     Revision;
    };
    typedef std::map<std::string, LatestFirmwareCacheEntry> LatestFirmwareCacheMap;

    class LatestFirmwareCache : public SubSystemServer {
    public:
        static LatestFirmwareCache *instance() {
            static LatestFirmwareCache instance;
            return &instance;
        }

        int Start() override;
        void Stop() override;
        bool AddToCache(const std::string & DeviceType, const std::string & Revision, const std::string &Id, uint64_t TimeStamp);
        // void AddRevision(const std::string &Revision);
        bool FindLatestFirmware(const std::string &DeviceType, LatestFirmwareCacheEntry &Entry );
        void DumpCache();
        inline Types::StringSet GetRevisions() { std::lock_guard G(Mutex_); return RevisionSet_; };
        inline Types::StringSet GetDevices() { std::lock_guard G(Mutex_); return DeviceSet_; };
        bool IsLatest(const std::string &DeviceType, const std::string &Revision);

    private:
        LatestFirmwareCacheMap      Cache_;
        Types::StringSet            RevisionSet_;
        Types::StringSet            DeviceSet_;
        explicit LatestFirmwareCache() noexcept:
                SubSystemServer("FirmwareCache", "FIRMWARE-CACHE", "FirmwareCache")
        {
        }
    };

    inline LatestFirmwareCache * LatestFirmwareCache() { return LatestFirmwareCache::instance(); }
}


#endif //UCENTRALFMS_LATESTFIRMWARECACHE_H
