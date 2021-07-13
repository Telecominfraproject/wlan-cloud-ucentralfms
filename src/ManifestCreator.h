//
// Created by stephane bourque on 2021-06-02.
//

#ifndef UCENTRALFWS_MANIFESTCREATOR_H
#define UCENTRALFWS_MANIFESTCREATOR_H

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/core/auth/AWSCredentials.h>

#include "SubSystemServer.h"

namespace uCentral {

    struct S3BucketEntry {
        bool            Valid = false;
        std::string     S3Name;
        std::string     S3ContentManifest;
        uint64_t        S3TimeStamp = 0;
        uint64_t        S3Size = 0;
        std::string     Revision;
        std::string     Image;
        std::string     Compatible;
        uint64_t        Timestamp = 0;
        std::string     URI;
        void            Print() const;
    };
    typedef std::map<const std::string, S3BucketEntry>    S3BucketContent;

    class ManifestCreator : public SubSystemServer, Poco::Runnable {
    public:
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

        bool ComputeManifest(S3BucketContent & BucketContent);
        bool AddManifestToDB(S3BucketContent & BucketContent);
        bool InitBucket();
        bool ReadBucket(S3BucketContent & Bucket);
        bool GetBucketObjectContent(Aws::S3::S3Client &S3Client, const std::string &ObjectName, std::string & ObjectContent);
        void CloseBucket();
        void Print(const S3BucketContent &B);

    private:
        static ManifestCreator      *instance_;
        Poco::Thread                Worker_;
        std::atomic_bool            Running_ = false;
        std::atomic_bool            Updated_ = false;
        std::string                 CurrentManifest_;
        uint64_t                    ManifestVersion_=0;
        Aws::String                 S3BucketName_;
        Aws::String                 S3Region_;
        Aws::String                 S3Key_;
        Aws::String                 S3Secret_;
        uint64_t                    S3Retry_;
        Aws::Client::ClientConfiguration    AwsConfig_{"ARILIA"};
        Aws::Auth::AWSCredentials           AwsCreds_;

        ManifestCreator() noexcept:
                SubSystemServer("ManifestCreator", "MANIFEST-MGR", "manifestcreator") {
        }
    };

    inline ManifestCreator * ManifestCreator() { return ManifestCreator::instance(); };

}

#endif //UCENTRALFWS_MANIFESTCREATOR_H
