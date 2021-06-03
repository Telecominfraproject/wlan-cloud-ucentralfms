//
// Created by stephane bourque on 2021-06-02.
//

#ifndef UCENTRALFWS_UMANIFESTCREATOR_H
#define UCENTRALFWS_UMANIFESTCREATOR_H

#include "uSubSystemServer.h"
#include "s3bucketreader.h"

namespace uCentral::ManifestCreator {
    int Start();
    void Stop();
    bool Update();

    class Service : public uSubSystemServer, Poco::Runnable {
    public:

        Service() noexcept;
        friend int Start();
        friend void Stop();
        friend bool Update();

        static Service *instance() {
            if (instance_ == nullptr) {
                instance_ = new Service;
            }
            return instance_;
        }
        void run() override;

    private:
        static Service           *instance_;
        Poco::Thread              Worker_;
        std::atomic_bool          Running_ = false;
        std::atomic_bool          Updated_ = false;
        std::string               CurrentManifest_;
        uint64_t                  ManifestVersion_=0;
        S3BucketReader            BucketReader_;

        int Start() override;
        void Stop() override;

        bool Update();
        bool ComputeManifest(uCentral::BucketContent & BucketContent);
    };
}

#endif //UCENTRALFWS_UMANIFESTCREATOR_H
