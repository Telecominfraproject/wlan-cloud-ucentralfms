//
// Created by stephane bourque on 2021-06-02.
//

#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Stringifier.h"

#include "ManifestCreator.h"
#include "Utils.h"

#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/GetObjectRequest.h>

#include "Daemon.h"
#include "StorageService.h"
#include "LatestFirmwareCache.h"

namespace uCentral {
    class ManifestCreator *ManifestCreator::instance_ = nullptr;

    void ManifestCreator::run() {
        Running_ = true;

        while(Running_) {
            Poco::Thread::trySleep(10000);
            if(!Running_)
                break;
            std::cout << "About to read bucket..." << std::endl;
            S3BucketContent BucketList;
            ReadBucket(BucketList);
            if(!Running_)
                break;
            std::cout << "Bucket read: " << BucketList.size() << std::endl;
            ComputeManifest(BucketList);
            AddManifestToDB(BucketList);
            // LatestFirmwareCache()->DumpCache();
            // Print(BucketList);
        }
    }

    bool ManifestCreator::ComputeManifest(S3BucketContent &BucketContent) {

        for(auto &[Name,Entry]:BucketContent) {
            try {
                Poco::JSON::Parser  P;
                auto ParsedContent = P.parse(Entry.S3ContentManifest).extract<Poco::JSON::Object::Ptr>();

                if( ParsedContent->has("image") &&
                    ParsedContent->has("compatible") &&
                    ParsedContent->has("revision") &&
                    ParsedContent->has("timestamp"))
                {
                    Entry.Timestamp = ParsedContent->get("timestamp");
                    Entry.Compatible = ParsedContent->get("compatible").toString();
                    Entry.Revision = ParsedContent->get("revision").toString();
                    Entry.Image = ParsedContent->get("image").toString();
                    if(Entry.Image!=Name) {
                        Logger_.error(Poco::format("MANIFEST(%s): Image name does not match manifest name (%s).",Name,Entry.Image));
                        continue;
                    }
                    Entry.Valid = true;

                } else {
                    Logger_.error(Poco::format("MANIFEST(%s): Entry does not have a valid JSON manifest.",Name));
                }
            } catch (const Poco::Exception  &E ) {
                Logger_.log(E);
            }
        }
        return true;
    }

    bool ManifestCreator::AddManifestToDB(S3BucketContent & BucketContent) {

        for(auto &[Release,BucketEntry]:BucketContent) {
            FMSObjects::Firmware    F;
            auto R = Release;
            if(!Storage()->GetFirmwareByName(R,BucketEntry.Compatible,F)) {
                F.id = Daemon()->CreateUUID();
                F.release = Release;
                F.size = BucketEntry.S3Size;
                F.created = std::time(nullptr);
                F.imageDate = BucketEntry.S3TimeStamp;
                F.image = BucketEntry.S3Name;
                F.uri = BucketEntry.URI;
                F.revision = BucketEntry.Revision;
                F.deviceType = BucketEntry.Compatible;
                if(Storage()->AddFirmware(F)) {
                    std::cout << "Adding " << Release << std::endl;
                } else {
                    std::cout << "Could not add firmware..." << Release << std::endl;
                }
            }
        }
        return true;
    }

    int ManifestCreator::Start() {
        S3BucketName_ = Daemon()->ConfigGetString("s3.bucketname");
        S3Region_ = Daemon()->ConfigGetString("s3.region");
        S3Secret_ = Daemon()->ConfigGetString("s3.secret");
        S3Key_ = Daemon()->ConfigGetString("s3.key");
        S3Retry_ = Daemon()->ConfigGetInt("s3.retry",60);

        AwsConfig_.enableTcpKeepAlive = true;
        AwsConfig_.enableEndpointDiscovery = true;
        AwsConfig_.useDualStack = true;
        if(!S3Region_.empty())
            AwsConfig_.region = S3Region_;
        AwsCreds_.SetAWSAccessKeyId(S3Key_);
        AwsCreds_.SetAWSSecretKey(S3Secret_);

        Worker_.start(*this);
        return 0;
    }

    void ManifestCreator::Stop() {
        if(Running_) {
            Running_ = false;
            Worker_.wakeUp();
            Worker_.join();
        }
    }

    bool ManifestCreator::Update() {
        Worker_.wakeUp();
        return true;
    }

    void ManifestCreator::CloseBucket() {
    }

    bool ManifestCreator::GetBucketObjectContent(Aws::S3::S3Client &S3Client, const std::string &ObjectName,
                                                 std::string &ObjectContent) {
        Aws::S3::Model::GetObjectRequest Request;
        Request.SetBucket(S3BucketName_.c_str());
        Request.SetKey(ObjectName.c_str());

        Aws::S3::Model::GetObjectOutcome get_object_outcome = S3Client.GetObject(Request);

        if (get_object_outcome.IsSuccess())
        {
            auto & FileData = get_object_outcome.GetResultWithOwnership().GetBody();
            std::string O;
            std::ostringstream OS(O);
            OS << FileData.rdbuf();
            ObjectContent = OS.str();
            return true;
        }
        return false;
    }

    bool ManifestCreator::ReadBucket(S3BucketContent & Bucket) {
        static const std::string JSON(".json");
        static const std::string UPGRADE("-upgrade.bin");

        std::string     URIBase = "https://";
        URIBase += Daemon()->ConfigGetString("s3.bucket.uri");

        Bucket.clear();

        Aws::S3::Model::ListObjectsRequest Request;
        Request.WithBucket(S3BucketName_.c_str());
        Aws::S3::S3Client S3Client(AwsCreds_,AwsConfig_);

        auto Outcome = S3Client.ListObjects(Request);

        if(Outcome.IsSuccess()) {
//            std::cout << "Success..." << std::endl;
//            std::cout << "Objects: ." ;
            Aws::Vector<Aws::S3::Model::Object> objects = Outcome.GetResult().GetContents();
            for (const auto &Object : objects) {
                Poco::Path  FileName(Object.GetKey().c_str());
                // std::cout << "Object: " << Object.GetKey() << std::endl;
//                std::cout << "." << std::flush;
                if(!Running_)
                    return false;
                if (FileName.getExtension() == "json") {
                    std::string Release = FileName.getBaseName();
                    std::string Content;
                    if (GetBucketObjectContent(S3Client, FileName.getFileName(), Content)) {
                        Poco::JSON::Parser P;
                        auto ParsedContent = P.parse(Content).extract<Poco::JSON::Object::Ptr>();
                        if (ParsedContent->has("image") &&
                            ParsedContent->has("compatible") &&
                            ParsedContent->has("revision") &&
                            ParsedContent->has("timestamp")) {
                            auto It = Bucket.find(Release);
                            if (It != Bucket.end()) {
                                It->second.Timestamp = ParsedContent->get("timestamp");
                                It->second.Compatible = ParsedContent->get("compatible").toString();
                                It->second.Revision = ParsedContent->get("revision").toString();
                                It->second.Image = ParsedContent->get("image").toString();
                                It->second.S3ContentManifest = Content;
                            } else {
                                Bucket.emplace(Release, S3BucketEntry{
                                        .S3ContentManifest = Content,
                                        .Revision = ParsedContent->get("revision").toString(),
                                        .Image = ParsedContent->get("image").toString(),
                                        .Compatible = ParsedContent->get("compatible").toString(),
                                        .Timestamp = ParsedContent->get("timestamp")});
                            }
                        }
                    }
                } else if (FileName.getExtension() == "bin") {
                    const auto & ReleaseName = FileName.getFileName();
                    std::string Release = ReleaseName.substr(0, ReleaseName.size() - UPGRADE.size());
                    auto It = Bucket.find(ReleaseName);
                    if(It != Bucket.end()) {
                        It->second.S3TimeStamp = (uint64_t ) (Object.GetLastModified().Millis()/1000);
                        It->second.S3Size = Object.GetSize();
                        It->second.S3Name = ReleaseName;
                        It->second.URI = URIBase + ReleaseName;
                    } else {
                        Bucket.emplace(Release, S3BucketEntry{
                                .S3Name = ReleaseName,
                                .S3TimeStamp = (uint64_t ) (Object.GetLastModified().Millis()/1000),
                                .S3Size = (uint64_t ) Object.GetSize(),
                                .URI = URIBase + ReleaseName });
                    }
                } else {
                    // std::cout << "Ignoring " << FileName.getFileName() << std::endl;
                }
            }
        } else {
            Logger_.error(Poco::format("Problem with connecting: %s",Outcome.GetError()));
        }
        return true;
    }

    void S3BucketEntry::Print() const {
        if(Valid) {
            std::cout << "  Name: " << S3Name << std::endl;
            std::cout << "  Size: " << S3Size << std::endl;
            std::cout << "  Date: " << S3TimeStamp << std::endl;
            std::cout << "  Latest: " << S3ContentManifest << std::endl;
            std::cout << "  Image: " << Image << std::endl;
            std::cout << "  Revision: " << Revision << std::endl;
            std::cout << "  Compatible: " << Compatible << std::endl;
            std::cout << "  Timestamp: " << Timestamp << std::endl;
            std::cout << "  URI: " << URI << std::endl;
        } else {

        }
    }

    void Print(const S3BucketContent &B) {
        for(const auto &[Name,Entry]:B) {
            std::cout << "Release:" << Name << std::endl;
            Entry.Print();
        }
    }

}
