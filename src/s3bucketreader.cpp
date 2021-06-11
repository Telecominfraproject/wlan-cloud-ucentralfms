//
// Created by stephane bourque on 2021-06-02.
//

#include "s3bucketreader.h"
#include "Daemon.h"

#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/GetObjectRequest.h>

#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"

#include "Utils.h"

namespace uCentral {

    bool S3BucketReader::Initialize() {
        S3BucketName_ = uCentral::ServiceConfig::GetString("s3.bucketname");
        S3Region_ = uCentral::ServiceConfig::GetString("s3.region");
        S3Secret_ = uCentral::ServiceConfig::GetString("s3.secret");
        S3Key_ = uCentral::ServiceConfig::GetString("s3.key");
        S3Retry_ = uCentral::ServiceConfig::GetInt("s3.retry",60);

        AwsConfig_.enableTcpKeepAlive = true;
        AwsConfig_.enableEndpointDiscovery = true;
        AwsConfig_.useDualStack = true;
        if(!S3Region_.empty())
            AwsConfig_.region = S3Region_;
        AwsCreds_.SetAWSAccessKeyId(S3Key_);
        AwsCreds_.SetAWSSecretKey(S3Secret_);

        return true;
    }

    void S3BucketReader::Close() {
    }

    bool S3BucketReader::GetObjectContent(Aws::S3::S3Client &S3Client, const std::string &ObjectName, std::string & ObjectContent) {
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

    bool S3BucketReader::ReadBucket() {

        static const std::string JSON(".json");
        static const std::string UPGRADE("-upgrade.bin");

        std::string     URIBase = "https://";
                        URIBase += uCentral::ServiceConfig::GetString("s3.bucket.uri/");

        BucketContent_.clear();

        Aws::S3::Model::ListObjectsRequest Request;
        Request.WithBucket(S3BucketName_.c_str());
        Aws::S3::S3Client S3Client(AwsCreds_,AwsConfig_);

        auto Outcome = S3Client.ListObjects(Request);

        if(Outcome.IsSuccess()) {
            Aws::Vector<Aws::S3::Model::Object> objects = Outcome.GetResult().GetContents();
            for (const auto &Object : objects) {
                std::string FileName{Object.GetKey()};
                if (FileName.size() > JSON.size() && FileName.substr(FileName.size() - JSON.size()) == JSON) {
                    std::string Release = FileName.substr(0, FileName.size() - JSON.size());
                    std::string Content;
                    if (GetObjectContent(S3Client, FileName, Content)) {
                        Poco::JSON::Parser P;
                        auto ParsedContent = P.parse(Content).extract<Poco::JSON::Object::Ptr>();
                        if (ParsedContent->has("image") &&
                            ParsedContent->has("compatible") &&
                            ParsedContent->has("revision") &&
                            ParsedContent->has("timestamp")) {
                            auto It = BucketContent_.find(Release);
                            if (It != BucketContent_.end()) {
                                It->second.Timestamp = ParsedContent->get("timestamp");
                                It->second.Compatible = ParsedContent->get("compatible").toString();
                                It->second.Revision = ParsedContent->get("revision").toString();
                                It->second.Image = ParsedContent->get("image").toString();
                                It->second.S3ContentManifest = Content;
                            } else {
                                BucketContent_.emplace(Release, BucketEntry{
                                        .S3ContentManifest = Content,
                                        .Revision = ParsedContent->get("revision").toString(),
                                        .Image = ParsedContent->get("image").toString(),
                                        .Compatible = ParsedContent->get("compatible").toString(),
                                        .Timestamp = ParsedContent->get("timestamp")});
                            }
                        }
                    }
                } else if (FileName.size() > UPGRADE.size() && FileName.substr(FileName.size() - UPGRADE.size()) == UPGRADE) {
                    std::string Release = FileName.substr(0, FileName.size() - UPGRADE.size());
                    auto It = BucketContent_.find(Release);
                    if(It != BucketContent_.end()) {
                        It->second.S3TimeStamp = (uint64_t ) (Object.GetLastModified().Millis()/1000);
                        It->second.S3Size = Object.GetSize();
                        It->second.S3Name = FileName;
                        It->second.URI = URIBase + FileName;
                    } else {
                        BucketContent_.emplace(Release, BucketEntry{
                                                            .S3Name = FileName,
                                                            .S3TimeStamp = (uint64_t ) (Object.GetLastModified().Millis()/1000),
                                                            .S3Size = (uint64_t ) Object.GetSize(),
                                                            .URI = URIBase + FileName });
                    }
                }
            }
        }
        return true;
    }

    const BucketContent & S3BucketReader::Bucket() const {
        return BucketContent_;
    };

    void BucketEntry::Print() const {
        std::cout << "  Name: " << S3Name << std::endl;
        std::cout << "  Size: " << S3Size << std::endl;
        std::cout << "  Date: " << S3TimeStamp << std::endl;
        std::cout << "  Latest: " << S3ContentManifest << std::endl;
        std::cout << "  Image: " << Image << std::endl;
        std::cout << "  Revision: " << Revision << std::endl;
        std::cout << "  Compatible: " << Compatible << std::endl;
        std::cout << "  Timestamp: " << Timestamp << std::endl;
        std::cout << "  URI: " << URI << std::endl;
     }

    void Print(const uCentral::BucketContent &B) {
        for(const auto &[Name,Entry]:B) {
            std::cout << "Release:" << Name << std::endl;
            Entry.Print();
        }
    }
}