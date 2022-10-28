//
// Created by stephane bourque on 2021-10-04.
//

#include "AutoUpdater.h"
#include "SDK/Prov_SDK.h"
#include "SDK/GW_SDK.h"
#include "LatestFirmwareCache.h"
#include "StorageService.h"

#include "framework/MicroServiceFuncs.h"
#include "framework/utils.h"

#include "fmt/format.h"

namespace OpenWifi {

    int AutoUpdater::Start() {
        poco_information(Logger(),"Starting...");
        AutoUpdaterEnabled_ = MicroServiceConfigGetBool("autoupdater.enabled", false);
        if(AutoUpdaterEnabled_) {
            Running_ = false;
            AutoUpdaterFrequency_ = MicroServiceConfigGetInt("autoupdater.frequency",600);
            AutoUpdaterCallBack_ = std::make_unique<Poco::TimerCallback<AutoUpdater>>(*this, &AutoUpdater::onTimer);
            Timer_.setStartInterval(5 * 60 * 1000);  // first run in 5 minutes
            Timer_.setPeriodicInterval(AutoUpdaterFrequency_ * 1000);
            Timer_.start(*AutoUpdaterCallBack_);
        }
        return 0;
    }

    void AutoUpdater::Stop() {
        poco_information(Logger(),"Stopping...");
        Running_ = false;
        if(AutoUpdaterEnabled_) {
            Timer_.stop();
        }
        poco_information(Logger(),"Stopped...");
    }

    void AutoUpdater::ToBeUpgraded(std::string serialNumber, std::string DeviceType) {
        if(!AutoUpdaterEnabled_)
            return;
        std::lock_guard G(Mutex_);
        Queue_.emplace_back(std::make_pair(std::move(serialNumber),std::move(DeviceType)));
    }

    void AutoUpdater::onTimer([[maybe_unused]] Poco::Timer & timer) {
        Utils::SetThreadName("auto-updater");
        Running_ = true;
        std::unique_lock    L(Mutex_);
        while(!Queue_.empty() && Running_) {
            auto Entry = Queue_.front();
            Queue_.pop_front();
            try {
                poco_debug(Logger(),fmt::format("Preparing to upgrade {}",Entry.first));
                auto CacheEntry = Cache_.find(Entry.first);
                uint64_t now = Utils::Now();
                std::string firmwareUpgrade;
                if(CacheEntry == Cache_.end() || (CacheEntry->second.LastCheck-now)>300) {
                    //  get the firmware settings for that device.
                    SerialCache     C;
                    C.LastCheck = now;
                    bool        firmwareRCOnly;
                    if(OpenWifi::SDK::Prov::GetFirmwareOptions(Entry.first, firmwareUpgrade, firmwareRCOnly)) {
                        poco_debug(Logger(),fmt::format("Found firmware options for {}",Entry.first));
                        C.firmwareRCOnly = firmwareRCOnly;
                        C.firmwareUpgrade = firmwareUpgrade;
                    } else {
                        poco_debug(Logger(),fmt::format("Found no firmware options for {}",Entry.first));
                        C.firmwareRCOnly = firmwareRCOnly;
                        C.firmwareUpgrade = firmwareUpgrade;
                    }
                    Cache_[Entry.first] = C;
                } else {

                }

                if(firmwareUpgrade=="no") {
                    poco_information(Logger(),fmt::format("Device {} not upgradable. Provisioning service settings.",Entry.first));
                    continue;
                }

                LatestFirmwareCacheEntry    fwEntry;
                FMSObjects::Firmware        fwDetails;
                auto LF = LatestFirmwareCache()->FindLatestFirmware(Entry.second, fwEntry );
                if(LF) {
                    if(StorageService()->FirmwaresDB().GetFirmware(fwEntry.Id,fwDetails)) {
                        //  send the command to upgrade this device...
                        poco_information(Logger(),fmt::format("Upgrading {} to version {}", Entry.first, fwEntry.Revision));
                        if(OpenWifi::SDK::GW::SendFirmwareUpgradeCommand(Entry.first,fwDetails.uri)) {
                            poco_information(Logger(),fmt::format("Upgrade command sent for {}",Entry.first));
                        } else {
                            poco_information(Logger(),fmt::format("Upgrade command not sent for {}",Entry.first));
                        }
                    } else {
                        poco_information(Logger(),fmt::format("Firmware for device {} ({}) cannot be found.", Entry.first, Entry.second ));
                    }
                } else {
                    poco_information(Logger(),fmt::format("Firmware for device {} ({}) cannot be found.", Entry.first, Entry.second ));
                }
            } catch (...) {
                poco_information(Logger(),fmt::format("Exception during auto update for device {}.", Entry.first ));
            }
        }
    }

    void AutoUpdater::reinitialize([[maybe_unused]] Poco::Util::Application &self) {
        poco_information(Logger(),"Reinitializing.");
        Reset();
    }
}