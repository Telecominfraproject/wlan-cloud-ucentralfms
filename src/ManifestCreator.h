//
// Created by stephane bourque on 2021-06-02.
//

#ifndef UCENTRALFWS_MANIFESTCREATOR_H
#define UCENTRALFWS_MANIFESTCREATOR_H

#include "SubSystemServer.h"
#include "s3bucketreader.h"

namespace uCentral {

    class ManifestCreator : public SubSystemServer, Poco::Runnable {
    public:

        ManifestCreator() noexcept;

        static ManifestCreator *instance() {
            if (instance_ == nullptr) {
                instance_ = new ManifestCreator;
            }
            return instance_;
        }

        void run() override;
        int Start() override;
        void Stop() override;
        bool Update();
        bool ComputeManifest(uCentral::BucketContent & BucketContent);

    private:
        static ManifestCreator    *instance_;
        Poco::Thread              Worker_;
        std::atomic_bool          Running_ = false;
        std::atomic_bool          Updated_ = false;
        std::string               CurrentManifest_;
        uint64_t                  ManifestVersion_=0;
        S3BucketReader            BucketReader_;
    };

    inline ManifestCreator * ManifestCreator() { return ManifestCreator::instance(); };

}

#endif //UCENTRALFWS_MANIFESTCREATOR_H
