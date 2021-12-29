//
// Created by stephane bourque on 2021-06-02.
//

#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Stringifier.h"

#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/ListObjectsV2Request.h>
#include <aws/s3/model/GetObjectRequest.h>

#include "ManifestCreator.h"
#include "StorageService.h"
#include "LatestFirmwareCache.h"

namespace OpenWifi {

    void ManifestCreator::run() {
        Running_ = true;
        bool FirstRun = true;

        while(Running_) {
            Poco::Thread::trySleep(FirstRun ? 10000 : DBRefresh_*1000);
            if(!Running_)
                break;
            FirstRun = false;
            Logger().information("Performing DB refresh");
            S3BucketContent BucketList;
            StorageService()->FirmwaresDB().RemoveOldFirmware();
            ReadBucket(BucketList);
            if(!Running_)
                break;
            Logger().information(Poco::format("Found %Lu firmware entries in S3 repository.",(uint64_t)BucketList.size()));
            ComputeManifest(BucketList);
            AddManifestToDB(BucketList);
        }
    }

    bool ManifestCreator::ComputeManifest(S3BucketContent &BucketContent) {

        uint64_t Limit = std::time(nullptr) - MaxAge_, Rejected=0, Accepted=0, BadFormat=0, MissingJson=0;
        for(auto &[Name,Entry]:BucketContent) {
            std::string C = Entry.S3ContentManifest;

            try {
                Poco::JSON::Parser  P;
                auto ParsedContent = P.parse(Entry.S3ContentManifest).extract<Poco::JSON::Object::Ptr>();

                if( ParsedContent->has("image") &&
                    ParsedContent->has("compatible") &&
                    ParsedContent->has("revision") &&
                    ParsedContent->has("timestamp"))
                {
                    Entry.Timestamp = ParsedContent->get("timestamp");
                    if(Entry.Timestamp>Limit) {
                        Entry.Compatible = ParsedContent->get("compatible").toString();
                        Entry.Revision = ParsedContent->get("revision").toString();
                        Entry.Image = ParsedContent->get("image").toString();
                        auto FullNme = Name + "-upgrade.bin";
                        if(FullNme!=Entry.Image) {
                            Logger().error(Poco::format("MANIFEST(%s): Image name does not match manifest name (%s).",Name,Entry.Image));
                            Entry.Valid = false;
                            BadFormat++;
                            continue;
                        }
                        Accepted++;
                        Entry.Valid = true;
                    } else {
                        Rejected++;
                        Entry.Valid = false;
                    }
                } else {
                    Logger().error(Poco::format("MANIFEST(%s): Entry does not have a valid JSON manifest.",Name));
                    MissingJson++;
                    Entry.Valid = false;
                }
            } catch (const Poco::Exception  &E ) {
                Logger().log(E);
            }
        }

        Logger().information(Poco::format("Accepted %Lu firmwares.", Accepted));
        Logger().information(Poco::format("Rejected %Lu too old firmwares.", Rejected));
        Logger().information(Poco::format("Rejected %Lu bad JSON.", BadFormat));
        Logger().information(Poco::format("Rejected %Lu missing JSON.", MissingJson));

        return true;
    }

    bool ManifestCreator::AddManifestToDB(S3BucketContent & BucketContent) {

        for(auto &[Release,BucketEntry]:BucketContent) {
            FMSObjects::Firmware    F;
            auto R = Release;

            // skip staging releases.
            if(BucketEntry.URI.find("-staging-")!=std::string::npos)
                continue;

            if(BucketEntry.Valid && !StorageService()->FirmwaresDB().GetFirmwareByName(R,BucketEntry.Compatible,F)) {
                F.id = MicroService::instance().CreateUUID();
                F.release = Release;
                F.size = BucketEntry.S3Size;
                F.created = std::time(nullptr);
                F.imageDate = BucketEntry.S3TimeStamp;
                F.image = BucketEntry.S3Name;
                F.uri = BucketEntry.URI;
                F.revision = BucketEntry.Revision;
                F.deviceType = BucketEntry.Compatible;
                if(StorageService()->FirmwaresDB().AddFirmware(F)) {
                    Logger().information(Poco::format("Adding firmware '%s'",Release));
                } else {
                }
            }
        }
        return true;
    }

    int ManifestCreator::Start() {
        S3BucketName_ = MicroService::instance().ConfigGetString("s3.bucketname");
        S3Region_ = MicroService::instance().ConfigGetString("s3.region");
        S3Secret_ = MicroService::instance().ConfigGetString("s3.secret");
        S3Key_ = MicroService::instance().ConfigGetString("s3.key");
        S3Retry_ = MicroService::instance().ConfigGetInt("s3.retry",60);

        DBRefresh_ = MicroService::instance().ConfigGetInt("firmwaredb.refresh",30*60);
        MaxAge_ = MicroService::instance().ConfigGetInt("firmwaredb.maxage",90) * 24 * 60 * 60;

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
        URIBase += MicroService::instance().ConfigGetString("s3.bucket.uri");

        Bucket.clear();

        Aws::S3::Model::ListObjectsV2Request Request;
        Request.WithBucket(S3BucketName_.c_str());
        Aws::S3::S3Client S3Client(AwsCreds_,AwsConfig_);
        Request.SetMaxKeys(100);
        Aws::S3::Model::ListObjectsV2Outcome Outcome;

        bool isDone=false;
        int Count=0, Runs=0;

        while(!isDone) {
            Outcome = S3Client.ListObjectsV2(Request);
            if(!Outcome.IsSuccess()) {
                Logger().error(Poco::format("Error while doing ListObjectsV2: %s, %s",
                                           std::string{Outcome.GetError().GetExceptionName()},
                                           std::string{Outcome.GetError().GetMessage()}));
                return false;
            }
            Aws::Vector<Aws::S3::Model::Object> objects = Outcome.GetResult().GetContents();
            Runs++;
            for (const auto &Object : objects) {
                Count++;
                // std::cout << "Run: " << Runs << "  Count: " << Count << std::endl;
                Poco::Path FileName(Object.GetKey().c_str());
                if (!Running_)
                    return false;
                if (FileName.getExtension() == "json") {
                    std::string Release = FileName.getBaseName();
                    std::string Content;

                    if (GetBucketObjectContent(S3Client, FileName.getFileName(), Content)) {
                        // std::cout << "Object: " << FileName.getFileName() << std::endl;
                        // std::cout << "Content: " << Content << std::endl;
                        Poco::JSON::Parser P;
                        auto ParsedContent = P.parse(Content).extract<Poco::JSON::Object::Ptr>();
                        if (ParsedContent->has("image") &&
                            ParsedContent->has("compatible") &&
                            ParsedContent->has("revision") &&
                            ParsedContent->has("timestamp")) {
                            auto It = Bucket.find(Release);
                            uint64_t TimeStamp = ParsedContent->get("timestamp");
                            auto Compatible = ParsedContent->get("compatible").toString();
                            auto Revision = ParsedContent->get("revision").toString();
                            // std::cout << "Revision from bucket in JSON" << Revision << std::endl;
                            auto Image = ParsedContent->get("image").toString();
                            if (It != Bucket.end()) {
                                It->second.Timestamp = TimeStamp;
                                It->second.Compatible = Compatible;
                                It->second.Revision = Revision;
                                It->second.Image = Image;
                                It->second.S3ContentManifest = Content;
                            } else {
                                Bucket.emplace(Release, S3BucketEntry{
                                        .S3ContentManifest = Content,
                                        .Revision = Revision,
                                        .Image = Image,
                                        .Compatible = Compatible,
                                        .Timestamp = TimeStamp});
                            }
                        }
                    }
                } else if (FileName.getExtension() == "bin") {
                    //  we must remove -upgrade, so
                    const auto &ReleaseName = FileName.getBaseName().substr(0, FileName.getBaseName().size() - 8);
                    auto It = Bucket.find(ReleaseName);
                    auto S3TimeStamp = (uint64_t) (Object.GetLastModified().Millis() / 1000);
                    uint64_t S3Size = Object.GetSize();
                    std::string URI = URIBase + "/" + FileName.getFileName();
                    if (It != Bucket.end()) {
                        It->second.S3TimeStamp = S3TimeStamp;
                        It->second.S3Size = S3Size;
                        It->second.S3Name = ReleaseName;
                        It->second.URI = URI;
                    } else {
                        Bucket.emplace(ReleaseName, S3BucketEntry{
                                .S3Name = ReleaseName,
                                .S3TimeStamp = S3TimeStamp,
                                .S3Size = S3Size,
                                .URI = URI});
                    }
                } else {
                    // std::cout << "Ignoring " << FileName.getFileName() << std::endl;
                }
            }

            isDone = !Outcome.GetResult().GetIsTruncated();
            if(!isDone) {
                // std::cout << "Going for next run..." << std::endl;
                // auto Token = Outcome.GetResult().GetContinuationToken();
                auto Token = Outcome.GetResult().GetNextContinuationToken();
                Request.SetContinuationToken(Token);
                // std::cout << "Continuation set..." << std::endl;
            }
        }

        // std::cout << "Count:" << Count << "  Runs:" << Runs << std::endl;
        if(!Outcome.IsSuccess()) {
            Logger().error(Poco::format("Error while doing ListObjectsV2: %s, %s",
                                       std::string{Outcome.GetError().GetExceptionName()},
                                       std::string{Outcome.GetError().GetMessage()}));
            return false;
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
