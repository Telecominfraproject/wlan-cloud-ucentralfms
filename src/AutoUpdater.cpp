//
// Created by stephane bourque on 2021-10-04.
//

#include "AutoUpdater.h"
#include "Prov_SDK.h"
#include "GW_SDK.h"
#include "Daemon.h"
#include "LatestFirmwareCache.h"
#include "StorageService.h"

namespace OpenWifi {
    class AutoUpdater * AutoUpdater::instance_ = nullptr;

    int AutoUpdater::Start() {
        Running_ = true;
        AutoUpdaterFrequency_ = Daemon()->ConfigGetInt("autoupdater.frequency",600);
        AutoUpdaterEnabled_ = Daemon()->ConfigGetBool("autoupdater.enabled", true);
        Thr_.start(*this);
        return 0;
    }

    void AutoUpdater::Stop() {
        Running_ = false;
        Thr_.wakeUp();
        Thr_.join();
    }

    void AutoUpdater::ToBeUpgraded(std::string serialNumber, std::string DeviceType) {
        std::lock_guard G(Mutex_);
        Queue_.emplace_back(std::make_pair(std::move(serialNumber),std::move(DeviceType)));
    }

    void AutoUpdater::run() {
        while(Running_) {
            Poco::Thread::trySleep(2000);
            if(!Running_)
                break;
            std::unique_lock    L(Mutex_);
            while(!Queue_.empty() && Running_) {
                auto Entry = Queue_.front();
                Queue_.pop_front();
                try {
                    Logger_.debug(Poco::format("Preparing to upgrade %s",Entry.first));
                    auto CacheEntry = Cache_.find(Entry.first);
                    uint64_t Now = std::time(nullptr);
                    std::string firmwareUpgrade;
                    bool        firmwareRCOnly;
                    if(CacheEntry == Cache_.end() || (CacheEntry->second.LastCheck-Now)>300) {
                        //  get the firmware settings for that device.
                        SerialCache     C;
                        C.LastCheck = Now;
                        if(OpenWifi::SDK::Prov::GetFirmwareOptions(Entry.first, firmwareUpgrade, firmwareRCOnly)) {
                            Logger_.debug(Poco::format("Found firmware options for %s",Entry.first));
                            C.firmwareRCOnly = firmwareRCOnly;
                            C.firmwareUpgrade = firmwareUpgrade;
                        } else {
                            Logger_.debug(Poco::format("Found no firmware options for %s",Entry.first));
                            C.firmwareRCOnly = firmwareRCOnly;
                            C.firmwareUpgrade = firmwareUpgrade;
                        }
                        Cache_[Entry.first] = C;
                    } else {

                    }

                    if(firmwareUpgrade=="no") {
                        Logger_.information(Poco::format("Device %s not upgradable. Provisioning service settings.",Entry.first));
                        continue;
                    }

                    LatestFirmwareCacheEntry    fwEntry;
                    FMSObjects::Firmware        fwDetails;
                    auto LF = LatestFirmwareCache()->FindLatestFirmware(Entry.second, fwEntry );
                    if(LF) {
                        if(Storage()->GetFirmware(fwEntry.Id,fwDetails)) {
                            //  send the command to upgrade this device...
                            Logger_.information(Poco::format("Upgrading %s to version %s", Entry.first, fwEntry.Revision));
                            if(OpenWifi::SDK::GW::SendFirmwareUpgradeCommand(Entry.first,fwDetails.uri)) {
                                Logger_.information(Poco::format("Upgrade command sent for %s",Entry.first));
                            } else {
                                Logger_.information(Poco::format("Upgrade command not sent for %s",Entry.first));
                            }
                        } else {
                            Logger_.information(Poco::format("Firmware for device %s (%s) cannot be found.", Entry.first, Entry.second ));
                        }
                    } else {
                        Logger_.information(Poco::format("Firmware for device %s (%s) cannot be found.", Entry.first, Entry.second ));
                    }
                } catch (...) {
                    Logger_.information(Poco::format("Exception during auto update for device %s.", Entry.first ));
                }
            }
        }
    }

    void AutoUpdater::reinitialize(Poco::Util::Application &self) {
        Logger_.information("Reinitializing.");
        Reset();
    }
}