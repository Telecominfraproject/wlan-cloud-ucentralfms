//
// Created by stephane bourque on 2021-07-26.
//

#include "FirmwareCache.h"

namespace uCentral {
    class FirmwareCache *FirmwareCache::instance_ = nullptr;

    int FirmwareCache::Start() {
        return 0;
    }

    void FirmwareCache::Stop() {

    }

    std::shared_ptr<FMSObjects::Firmware> GetFirmware(const std::string & DeviceType, const std::string & Revision) {
        return nullptr;
    }

    std::shared_ptr<FMSObjects::Firmware> AddFirmware(const FMSObjects::Firmware &F) {
        return nullptr;
    }

}
