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
                        const std::string &serialNumber, const std::string & DeviceType,
                        const std::string &Host, const std::string &Firmware) {

    }

    void DeviceCache::DumpCache() {

    }
}