//
// Created by stephane bourque on 2021-05-10.
//

#include "FWManager.h"

namespace uCentral::FWManager {
    Service *Service::instance_ = nullptr;

    int Start() {
        return Service::instance()->Start();
    }

    void Stop() {
        Service::instance()->Stop();
    }

    bool AddJob(const std::string &UUID) {
        return Service::instance()->AddJob(UUID);
    }


    Service::Service() noexcept:
            SubSystemServer("FirmwareMgr", "FWR-MGR", "firmwaremgr")
    {
    }

    int Service::Start() {
        SubMutexGuard Guard(Mutex_);

        Logger_.information("Starting ");
        Worker_.start(*this);

        return 0;
    }

    void Service::Stop() {
        SubMutexGuard Guard(Mutex_);

        Logger_.information("Stopping ");
        Running_ = false;
        Worker_.join();
    }

    void Service::run() {

        Running_ = true;

        while(Running_ && !Jobs_.empty()) {

            Poco::Thread::sleep(2000);
            std::string Job;
            {
                SubMutexGuard G(Mutex_);
                Job = Jobs_.front();
            }

            {
                SubMutexGuard G(Mutex_);
                Jobs_.pop();
            }
        }

    }

    bool Service::AddJob(const std::string &UUID) {
        SubMutexGuard Guard(Mutex_);

        Jobs_.push(UUID);

        return true;
    }

}   // namespace
