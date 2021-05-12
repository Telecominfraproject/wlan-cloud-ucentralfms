//
// Created by stephane bourque on 2021-05-11.
//

#ifndef UCENTRALFWS_UNOTIFICATIONMGR_H
#define UCENTRALFWS_UNOTIFICATIONMGR_H

#include <queue>

#include "uSubSystemServer.h"
#include "uAuthService.h"
#include "RESTAPI_objects.h"

namespace uCentral::NotificationMgr {

    int Start();
    void Stop();
    void Update();

    struct NotifyEndPoint {
        uCentral::Objects::Callback     Caller;
        uint64_t                        LasContact;
        uint64_t                        LastVersion;
    };

    class Service : public uSubSystemServer, Poco::Runnable {
    public:

        Service() noexcept;

        friend int Start();
        friend void Stop();
        friend void Update();

        static Service *instance() {
            if (instance_ == nullptr) {
                instance_ = new Service;
            }
            return instance_;
        }
        void run() override;

    private:
        static Service                          *instance_;
        Poco::Thread                            Worker_;
        std::atomic_bool                        Running_ = false;
        std::atomic_bool                        Updated_ = false;
        std::map<std::string,NotifyEndPoint>    EndPoints_;

        std::string                             CurrentManifest_;
        uint64_t                                ManifestVersion_;

        int Start() override;
        void Stop() override;
        void Update();

        void NotifyCallers();
        void SetVersion(const std::string &Manifest);
        bool SendManifest(const uCentral::Objects::Callback &Host);
    };

}   // namespace


#endif //UCENTRALFWS_UNOTIFICATIONMGR_H
