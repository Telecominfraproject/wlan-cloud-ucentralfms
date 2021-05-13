//
// Created by stephane bourque on 2021-05-10.
//

#include <iostream>
#include <fstream>

#include "Poco/File.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Stringifier.h"

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/Owner.h>
#include <aws/s3/model/AccessControlPolicy.h>
#include <aws/s3/model/PutBucketAclRequest.h>
#include <aws/s3/model/GetBucketAclRequest.h>

#include "uFWManager.h"
#include "uFileUploader.h"
#include "uFirmwareDS.h"
#include "uNotificationMgr.h"

#include "uStorageService.h"
#include "RESTAPI_objects.h"

namespace uCentral::FWManager {
    Service *Service::instance_ = nullptr;

    int Start() {
        return Service::instance()->Start();
    }

    void Stop() {
        Service::instance()->Stop();
    }

    bool AddJob(const std::string &UUID, const uCentral::Auth::APIKeyEntry & Entry) {
        return Service::instance()->AddJob(UUID, Entry);
    }

    Service::Service() noexcept:
            uSubSystemServer("FirmwareMgr", "FWR-MGR", "firmwaremgr")
    {
    }

    int Service::Start() {
        SubMutexGuard Guard(Mutex_);

        S3BucketName_ = uCentral::ServiceConfig::GetString("s3.bucketname");
        S3Region_ = uCentral::ServiceConfig::GetString("s3.region");
        S3Secret_ = uCentral::ServiceConfig::GetString("s3.secret");
        S3Key_ = uCentral::ServiceConfig::GetString("s3.key");
        S3Retry_ = uCentral::ServiceConfig::GetInt("s3.retry",60);

        Logger_.information("Starting ");
        Worker_.start(*this);

        return 0;
    }

    void Service::Stop() {
        SubMutexGuard Guard(Mutex_);

        Logger_.information("Stopping ");
        Running_ = false;
        Worker_.join();
    }

    void Service::run() {

        Running_ = true;

        const std::string & Path = uCentral::uFileUploader::Path();

        auto Uploads=0;

        while(Running_) {
            bool RemoveJob = false;

            if(Jobs_.empty()) {
                if (Uploads!=0) {
                    uCentral::NotificationMgr::Update();
                    Uploads = 0;
                }
                Poco::Thread::sleep(10000);
            } else {
                JobId JobEntry;
                {
                    SubMutexGuard G(Mutex_);
                    JobEntry = Jobs_.front();
                }

                try {
                    Poco::File JSONFileName(Path + "/" + JobEntry.UUID + "/latest-upgrade.json");

                    if (JSONFileName.exists() && JSONFileName.isFile()) {
                        std::ifstream  in(JSONFileName.path(),std::ios_base::in);

                        Poco::JSON::Parser parser;
                        Poco::JSON::Object::Ptr Obj = parser.parse(in).extract<Poco::JSON::Object::Ptr>();
                        Poco::DynamicStruct ds = *Obj;
                        in.close();

                        if( ds.contains("image") && ds.contains("compatible")
                            && ds.contains("revision") && ds.contains("timestamp")) {

                            //  let's see if the image file exists
                            std::string ImageName{ds["image"].toString()};
                            Poco::File  ImageFileName(Path + "/" + JobEntry.UUID + "/" + ImageName);
                            if(ImageFileName.exists() && ImageFileName.isFile()) {
                                Poco::File JSONRealFileName{ ImageFileName.path() + ".json"};
                                std::string JSONObjectName{ImageName+".json"};
                                JSONFileName.copyTo(JSONRealFileName.path());
                                Logger_.information(Poco::format("JOB(%s): Processing...",JobEntry.UUID));
                                if(SendToS3(JSONObjectName, JSONRealFileName.path(),
                                            ImageName, ImageFileName.path())) {
                                    RemoveJob = true;

                                    //  create the new firmware entry
                                    uCentral::Objects::Firmware F;

                                    F.UUID = uCentral::instance()->CreateUUID();
                                    F.Owner = JobEntry.Entry.Owner;
                                    F.FirmwareDate = ds["timestamp"];
                                    F.Size = ImageFileName.getSize();
                                    F.DownloadCount = 0;
                                    F.Uploaded = time(nullptr);
                                    F.Compatible = ds["compatible"].toString();
                                    F.FirmwareVersion = ds["revision"].toString();
                                    F.FirmwareFileName = ds["image"].toString();
                                    F.Uploader = JobEntry.Entry.Description;
                                    F.S3URI = "https://s3-" + S3Region_ + ".amazonaws.com/" + S3BucketName_ + "/" + ImageName;
                                    F.Latest = 1;

                                    if(uCentral::Storage::AddFirmware(F)) {
                                        Logger_.information(
                                                Poco::format("JOB(%s): Added to firmware DB.", JobEntry.UUID));
                                        RemoveJob = true;
                                        Uploads++;
                                    } else {
                                        Logger_.error(Poco::format("JOB(%s): Could not add the DB entry.",JobEntry.UUID));
                                    }
                                } else {
                                    RemoveJob = false;
                                }

                            } else {
                                RemoveJob = true;
                                Logger_.information(Poco::format("JOB(%s): Missing image file %s",JobEntry.UUID,ImageFileName.path()));
                            }

                        } else {
                            Logger_.information(Poco::format("JOB(%s): missing some JSON field(s).",JobEntry.UUID));
                            RemoveJob = true;
                        }
                    } else {
                        Logger_.information(Poco::format("JOB(%s): No JSON document.",JobEntry.UUID));
                        RemoveJob = true;
                    }
                } catch (const Poco::Exception &E) {
                    Logger_.log(E);
                    RemoveJob = true;
                }

                if(RemoveJob) {
                    SubMutexGuard G(Mutex_);
                    Jobs_.pop();
                }
            }
        }
    }

    bool Service::SendObjectToS3(std::shared_ptr<Aws::S3::S3Client> & Client, const std::string &ObjectName, const std::string & ObjectFileName) {
        try {
            Aws::S3::Model::PutObjectRequest request;

            request.SetBucket(S3BucketName_.c_str());
            request.SetKey(ObjectName.c_str());
            request.SetACL(Aws::S3::Model::ObjectCannedACL::public_read);

            std::cout << "Attempting to add " << ObjectName << " to the bucket " << S3BucketName_ << " in region "
                      << S3Region_ << std::endl;
            std::shared_ptr<Aws::IOStream> input_data =
                    Aws::MakeShared<Aws::FStream>("AriliaTag", ObjectFileName.c_str(),
                                                  std::ios_base::in | std::ios_base::binary);
            request.SetBody(input_data);

            Aws::S3::Model::PutObjectOutcome outcome =
                    Client->PutObject(request);

            if (outcome.IsSuccess()) {
                Logger_.information(Poco::format("S3-UPLOADER: uploaded %s", ObjectName));
                return true;
            } else {
                Logger_.error(Poco::format("S3-UPLOADER: could not upload %s. Exception: %s. Message: %s", ObjectName,
                                           outcome.GetError().GetExceptionName(), outcome.GetError().GetMessage()));
                return false;
            }
        } catch (...) {
            Logger_.error("Exception while uploading to S3.");
        }
        return false;
    }


    bool Service::SendToS3(const std::string & JSONObjectName , const std::string & JSONDocFileName,
                           const std::string & ImageObjectName, const std::string & ImageFileName) {
        Aws::SDKOptions options;
        Aws::InitAPI(options);
        {
            Aws::Client::ClientConfiguration config;
            if(!S3Region_.empty())
                config.region = S3Region_;

            std::shared_ptr<Aws::S3::S3Client> Client = Aws::MakeShared<Aws::S3::S3Client>(
                    "arilia.com",
                    Aws::Auth::AWSCredentials(S3Key_.c_str(),S3Secret_.c_str()), config);

            if( SendObjectToS3(Client,JSONObjectName,JSONDocFileName) &&
                SendObjectToS3(Client,ImageObjectName,ImageFileName) ) {
                return true;
            }
        }
        Aws::ShutdownAPI(options);
        return false;
    }

    bool Service::AddJob(const std::string &UUID, const uCentral::Auth::APIKeyEntry & Entry) {
        SubMutexGuard Guard(Mutex_);

        JobId   NewJob{ .UUID = UUID,
                        .Entry = Entry};

        Jobs_.push(NewJob);

        return true;
    }

}   // namespace
