//
// Created by stephane bourque on 2021-06-02.
//

#include "s3bucketreader.h"
#include "uFirmwareDS.h"

#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/GetObjectRequest.h>

#include "uUtils.h"

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

        BucketContent_.clear();

        Aws::S3::Model::ListObjectsRequest Request;
        Request.WithBucket(S3BucketName_.c_str());
        Aws::S3::S3Client S3Client(AwsCreds_,AwsConfig_);

        auto Outcome = S3Client.ListObjects(Request);

        if(Outcome.IsSuccess()) {
            Aws::Vector<Aws::S3::Model::Object> objects = Outcome.GetResult().GetContents();
            for (const auto & Object : objects)
            {
                std::string FileName{Object.GetKey()};

                //  if the file ends with .json, ignore it...
                if(FileName.substr(FileName.size()-5)!=".json") {
                    BucketEntry B;

                    B.S3Name = FileName;
                    B.S3Size = Object.GetSize();
                    B.S3TimeStamp = (Object.GetLastModified().Millis() / 1000);
                    BucketContent_[FileName] = B;
                }
            }
        }

        //  OK, now read the content os all the files that end with .json
        for(auto Element = BucketContent_.begin(); Element !=BucketContent_.end();) {
            std::string ObjectName = Element->second.S3Name + ".json";
            if(!GetObjectContent(S3Client,ObjectName,Element->second.S3ContentManifest)) {
                BucketContent_.erase(Element++);
            }
            else {
                Element++;
            }
        }
        return true;
    }

    const BucketContent & S3BucketReader::Bucket() const {
        return BucketContent_;
    };

    void BucketEntry::Print() const {
        std::cout << "Name: " << S3Name << std::endl;
        std::cout << "  Size: " << S3Size << std::endl;
        std::cout << "  Date: " << S3TimeStamp << std::endl;
        std::cout << "  Latest: " << S3ContentManifest << std::endl;
        std::cout << "  Image: " << Image << std::endl;
        std::cout << "  Revision: " << Revision << std::endl;
        std::cout << "  Compatible: " << Compatible << std::endl;
        std::cout << "  Timestamp: " << Timestamp << std::endl;
     }

    void Print(const uCentral::BucketContent &B) {
        for(const auto &[Name,Entry]:B)
            Entry.Print();
    }
}