//
// Created by stephane bourque on 2021-07-26.
//

#include "FirmwareCache.h"

namespace OpenWifi {

    int FirmwareCache::Start() {
        poco_information(Logger(),"Starting...");
        return 0;
    }

    void FirmwareCache::Stop() {
        poco_information(Logger(),"Stopping...");
        poco_information(Logger(),"Stopped...");
    }

    std::shared_ptr<FMSObjects::Firmware> GetFirmware([[maybe_unused]] const std::string & DeviceType, [[maybe_unused]] const std::string & Revision) {
        return nullptr;
    }

    std::shared_ptr<FMSObjects::Firmware> AddFirmware([[maybe_unused]] const FMSObjects::Firmware &F) {
        return nullptr;
    }

}
