//
// Created by stephane bourque on 2021-05-10.
//

#ifndef UCENTRALFWS_FWMANAGER_H
#define UCENTRALFWS_FWMANAGER_H

#include <queue>

#include "SubSystemServer.h"

namespace uCentral::FWManager {

    int Start();
    void Stop();
    bool AddJob(const std::string &UUID);

class Service : public SubSystemServer, Poco::Runnable {
    public:

        Service() noexcept;

        friend int Start();
        friend void Stop();

        static Service *instance() {
            if (instance_ == nullptr) {
                instance_ = new Service;
            }
            return instance_;
        }

        friend bool AddJob(const std::string &UUID);

        void run() override;

    private:
        static Service          *instance_;
        std::queue<std::string> Jobs_;
        Poco::Thread            Worker_;
        std::atomic_bool        Running_=false;
        int Start() override;
        void Stop() override;
        bool AddJob(const std::string &UUID);
    };

}   // namespace

#endif //UCENTRALFWS_FWMANAGER_H
