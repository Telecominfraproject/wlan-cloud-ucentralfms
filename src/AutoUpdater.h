//
// Created by stephane bourque on 2021-10-04.
//

#ifndef OWFMS_AUTOUPDATER_H
#define OWFMS_AUTOUPDATER_H

#include "framework/MicroService.h"
#include <deque>
#include "Poco/Util/Application.h"
#include "Poco/Timer.h"

namespace OpenWifi {

    class AutoUpdater : public SubSystemServer { // };, Poco::Runnable {
    public:

    struct SerialCache {
            uint64_t        LastCheck=0;
            std::string     firmwareUpgrade;
            bool            firmwareRCOnly=false;
        };

        static auto instance() {
            static auto instance_ = new AutoUpdater;
            return instance_;
        }

        int Start() override;
        void Stop() override;
        void ToBeUpgraded(std::string serialNumber, std::string DeviceType);
        inline void Reset() {
            std::lock_guard   G(Mutex_);
            Cache_.clear();
            Queue_.clear();
        }
        void reinitialize(Poco::Util::Application &self) final;
        void onTimer(Poco::Timer & timer);

    private:
        std::atomic_bool                                    Running_=false;
        std::map<std::string,SerialCache>                   Cache_;
        std::deque<std::pair<std::string,std::string>>      Queue_;
        uint64_t                                            AutoUpdaterFrequency_=600;
        bool                                                AutoUpdaterEnabled_=true;
        Poco::Timer                                         Timer_;
        std::unique_ptr<Poco::TimerCallback<AutoUpdater>>   AutoUpdaterCallBack_;

        explicit AutoUpdater() noexcept:
        SubSystemServer("AutoUpdater", "AUTO-UPDATER", "autoupdater")
            {
            }
    };

    inline auto AutoUpdater() { return AutoUpdater::instance(); }
}

#endif //OWFMS_AUTOUPDATER_H
