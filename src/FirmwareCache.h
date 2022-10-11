//
// Created by stephane bourque on 2021-07-26.
//

#pragma once

#include <map>
#include <memory>

#include "RESTObjects/RESTAPI_FMSObjects.h"
#include "framework/SubSystemServer.h"

namespace OpenWifi {

    typedef std::map<std::string,std::shared_ptr<FMSObjects::Firmware>> FirmwareCacheMap;

    class FirmwareCache: public SubSystemServer {
    public:
        static auto instance() {
            static auto instance_= new FirmwareCache;
            return instance_;
        }

        int Start() override;
        void Stop() override;

        std::shared_ptr<FMSObjects::Firmware> GetFirmware(const std::string & DeviceType, const std::string & Revision);
        std::shared_ptr<FMSObjects::Firmware> AddFirmware(const FMSObjects::Firmware &F);

    private:
        std::atomic_bool        Running_=false;
        FirmwareCacheMap        Cache_;
        explicit FirmwareCache() noexcept:
                SubSystemServer("FirmwareCache", "FIRMWARE-CACHE", "firmwarecache")
        {
        }
    };

    inline auto FirmwareCache() { return FirmwareCache::instance(); }

}
