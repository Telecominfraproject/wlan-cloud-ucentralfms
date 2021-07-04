//
// Created by stephane bourque on 2021-05-11.
//

#ifndef UCENTRALFWS_NOTIFICATIONMGR_H
#define UCENTRALFWS_NOTIFICATIONMGR_H

#include <queue>

#include "SubSystemServer.h"
#include "RESTAPI_objects.h"

namespace uCentral {

    struct NotifyEndPoint {
        uCentral::Objects::Callback     Caller;
        uint64_t                        LasContact;
        uint64_t                        LastVersion;
    };

    class NotificationMgr : public SubSystemServer, Poco::Runnable {
    public:

        NotificationMgr() noexcept;

        static NotificationMgr *instance() {
            if (instance_ == nullptr) {
                instance_ = new NotificationMgr;
            }
            return instance_;
        }
        void run() override;
        int Start() override;
        void Stop() override;
        void Update();

        void NotifyCallers();
        void SetVersion(const std::string &Manifest);
        bool SendManifest(const uCentral::Objects::Callback &Host);

    private:
        static NotificationMgr                 *instance_;
        Poco::Thread                            Worker_;
        std::atomic_bool                        Running_ = false;
        std::atomic_bool                        Updated_ = false;
        std::map<std::string,NotifyEndPoint>    EndPoints_;

        std::string                             CurrentManifest_;
        uint64_t                                ManifestVersion_=0;
    };

    inline NotificationMgr * NotificationMgr() { return NotificationMgr::instance(); };
}   // namespace


#endif //UCENTRALFWS_NOTIFICATIONMGR_H
