//
// Created by stephane bourque on 2021-06-02.
//

#include <list>

#ifndef UCENTRALFWS_S3BUCKETREADER_H
#define UCENTRALFWS_S3BUCKETREADER_H

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/core/auth/AWSCredentials.h>


namespace uCentral {
    //  pair: filename_root, content of root.latest
    struct BucketEntry {
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
    typedef std::map<std::string, BucketEntry>    BucketContent;
    void Print(const BucketContent &B);

    class S3BucketReader {
    public:
        bool Initialize();
        bool ReadBucket();
        const BucketContent & Bucket() const;
        bool GetObjectContent(Aws::S3::S3Client &S3Client, const std::string &ObjectName, std::string & ObjectContent);
        void Close();

    private:
        Aws::String             S3BucketName_;
        Aws::String             S3Region_;
        Aws::String             S3Key_;
        Aws::String             S3Secret_;
        uint64_t                S3Retry_;
        Aws::Client::ClientConfiguration    AwsConfig_{"ARILIA"};
        Aws::Auth::AWSCredentials           AwsCreds_;
        BucketContent           BucketContent_;
    };

}

#endif //UCENTRALFWS_S3BUCKETREADER_H
