//
// Created by stephane bourque on 2021-06-02.
//

#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Stringifier.h"

#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/ListObjectsV2Request.h>

#include "LatestFirmwareCache.h"
#include "ManifestCreator.h"
#include "StorageService.h"

#include "fmt/format.h"
#include "framework/utils.h"

namespace OpenWifi {

	void ManifestCreator::onTimer([[maybe_unused]] Poco::Timer &timer) {
		Utils::SetThreadName("manifest");
		RunUpdateTask();
	}

	bool ManifestCreator::RunUpdateTask() {
		if (!UpdateRunning_.test_and_set(std::memory_order_acquire)) {
			poco_information(Logger(), "Performing DB refresh");
			RunnerThread_.start(*this);
			return true;
		} else {
			poco_information(Logger(), "DB refresh already in progress");
			return false;
		}
	}

	void ManifestCreator::run() {
		S3BucketContent BucketList;
		StorageService()->FirmwaresDB().RemoveOldFirmware();
		ReadBucket(BucketList);
		poco_information(Logger(), fmt::format("Found {} firmware entries in S3 repository.",
											   BucketList.size()));
		ComputeManifest(BucketList);
		AddManifestToDB(BucketList);
		LastUpdate_ = Utils::Now();
		UpdateRunning_.clear(std::memory_order_release);
	}

	bool ManifestCreator::ComputeManifest(S3BucketContent &BucketContent) {

		uint64_t Limit = Utils::Now() - MaxAge_, Rejected = 0, Accepted = 0, BadFormat = 0,
				 MissingJson = 0;
		for (auto &[Name, Entry] : BucketContent) {
			std::string C = Entry.S3ContentManifest;

			try {
				Poco::JSON::Parser P;
				auto ParsedContent =
					P.parse(Entry.S3ContentManifest).extract<Poco::JSON::Object::Ptr>();

				if (ParsedContent->has("image") && ParsedContent->has("compatible") &&
					ParsedContent->has("revision") && ParsedContent->has("timestamp")) {
					Entry.Timestamp = ParsedContent->get("timestamp");
					if (Entry.Timestamp > Limit) {
						Entry.Compatible = ParsedContent->get("compatible").toString();
						Entry.Revision = ParsedContent->get("revision").toString();
						Entry.Image = ParsedContent->get("image").toString();
						auto FullNme = Name + "-upgrade.bin";
						if (FullNme != Entry.Image) {
							poco_error(
								Logger(),
								fmt::format(
									"MANIFEST({}): Image name does not match manifest name ({}).",
									Name, Entry.Image));
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
					poco_error(
						Logger(),
						fmt::format("MANIFEST({}): Entry does not have a valid JSON manifest.",
									Name));
					MissingJson++;
					Entry.Valid = false;
				}
			} catch (const Poco::Exception &E) {
				Logger().log(E);
			}
		}

		poco_information(Logger(), fmt::format("Accepted {} firmwares.", Accepted));
		poco_information(Logger(), fmt::format("Rejected {} too old firmwares.", Rejected));
		poco_information(Logger(), fmt::format("Rejected {} bad JSON.", BadFormat));
		poco_information(Logger(), fmt::format("Rejected {} missing JSON.", MissingJson));

		return true;
	}

	bool ManifestCreator::AddManifestToDB(S3BucketContent &BucketContent) {

		//  remove all staging names
		for (auto it = BucketContent.begin(); it != end(BucketContent);) {
			if (it->second.URI.find("-staging-") != std::string::npos) {
				it = BucketContent.erase(it);
			} else {
				++it;
			}
		}

		//  Now remove all DB entries that do not appear in the Latest manifest
		auto RemovedEntries =
			StorageService()->FirmwaresDB().RemoveOldDBEntriesNotInManifest(BucketContent);
		poco_information(Logger(), fmt::format("Removed {} DB entries that no longer are relevant.",
											   RemovedEntries));

		for (auto &[Release, BucketEntry] : BucketContent) {
			FMSObjects::Firmware F;
			auto R = Release;

			if (BucketEntry.Valid &&
				!StorageService()->FirmwaresDB().GetFirmwareByName(R, BucketEntry.Compatible, F)) {
				F.id = MicroServiceCreateUUID();
				F.release = Release;
				F.size = BucketEntry.S3Size;
				F.created = Utils::Now();
				F.imageDate = BucketEntry.S3TimeStamp;
				F.image = BucketEntry.Image;
				F.uri = BucketEntry.URI;
				F.revision = BucketEntry.Revision;
				F.deviceType = BucketEntry.Compatible;
				if (StorageService()->FirmwaresDB().AddFirmware(F)) {
					poco_information(Logger(),
									 fmt::format("Adding firmware '{}', size={}", Release, F.size));
				} else {
				}
			}
		}
		return true;
	}

	int ManifestCreator::Start() {
		Running_ = true;
		S3EndpointOverride_ = MicroServiceConfigGetString("s3.endpointOverride", "");
		S3EndpointHttps_ = MicroServiceConfigGetBool("s3.endpoint.https", true);
		S3UseVirtualAdressing_  = MicroServiceConfigGetBool("s3.useVirtualAdressing", true);
		S3BucketName_ = MicroServiceConfigGetString("s3.bucketname", "");
		S3Region_ = MicroServiceConfigGetString("s3.region", "");
		S3Secret_ = MicroServiceConfigGetString("s3.secret", "");
		S3Key_ = MicroServiceConfigGetString("s3.key", "");
		S3Retry_ = MicroServiceConfigGetInt("s3.retry", 60);

		DBRefresh_ = MicroServiceConfigGetInt("firmwaredb.refresh", 24 * 60 * 60);
		MaxAge_ = MicroServiceConfigGetInt("firmwaredb.maxage", 90) * 24 * 60 * 60;

		AwsConfig_.enableTcpKeepAlive = true;
		AwsConfig_.enableEndpointDiscovery = true;
		AwsConfig_.useDualStack = true;
		if(!S3EndpointHttps_)
			AwsConfig_.scheme = Aws::Http::Scheme::HTTP;
		if(!S3EndpointOverride_.empty()) {
			AwsConfig_.endpointOverride = Aws::String(S3EndpointOverride_);
			AwsConfig_.useDualStack = false;
		}
		if (!S3Region_.empty())
			AwsConfig_.region = S3Region_;
		AwsCreds_.SetAWSAccessKeyId(S3Key_);
		AwsCreds_.SetAWSSecretKey(S3Secret_);

		ManifestCreatorCallBack_ = std::make_unique<Poco::TimerCallback<ManifestCreator>>(
			*this, &ManifestCreator::onTimer);
		Timer_.setStartInterval(1 * 60 * 1000); // first run in 1 hour
		Timer_.setPeriodicInterval((long)(DBRefresh_ * 1000));
		Timer_.start(*ManifestCreatorCallBack_);

		return 0;
	}

	void ManifestCreator::Stop() {
		if (Running_) {
			Running_ = false;
			Timer_.stop();
		}
	}

	void ManifestCreator::CloseBucket() {}

	bool ManifestCreator::GetBucketObjectContent(Aws::S3::S3Client &S3Client,
												 const std::string &ObjectName,
												 std::string &ObjectContent) {
		Aws::S3::Model::GetObjectRequest Request;
		Request.SetBucket(S3BucketName_.c_str());
		Request.SetKey(ObjectName.c_str());

		Aws::S3::Model::GetObjectOutcome get_object_outcome = S3Client.GetObject(Request);

		if (get_object_outcome.IsSuccess()) {
			auto &FileData = get_object_outcome.GetResultWithOwnership().GetBody();
			std::string O;
			std::ostringstream OS(O);
			OS << FileData.rdbuf();
			ObjectContent = OS.str();
			return true;
		}
		return false;
	}

	bool ManifestCreator::ReadBucket(S3BucketContent &Bucket) {
		static const std::string JSON(".json");
		static const std::string UPGRADE("-upgrade.bin");

		std::string URIBase = "https://";
		URIBase += MicroServiceConfigGetString("s3.bucket.uri", "");

		Bucket.clear();

		Aws::S3::Model::ListObjectsV2Request Request;
		Request.WithBucket(S3BucketName_.c_str());
		Aws::S3::S3Client S3Client(AwsCreds_, AwsConfig_, Aws::Client::AWSAuthV4Signer::PayloadSigningPolicy::Never, S3UseVirtualAdressing_);
		Request.SetMaxKeys(100);
		Aws::S3::Model::ListObjectsV2Outcome Outcome;

		bool isDone = false;
		int Count = 0, Runs = 0;

		while (!isDone) {
			Outcome = S3Client.ListObjectsV2(Request);
			if (!Outcome.IsSuccess()) {
				poco_error(Logger(), fmt::format("Error while doing ListObjectsV2: {}, {}",
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
						if (ParsedContent->has("image") && ParsedContent->has("compatible") &&
							ParsedContent->has("revision") && ParsedContent->has("timestamp")) {
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
								Bucket.emplace(Release, S3BucketEntry{.Valid = false,
																	  .S3Name = "",
																	  .S3ContentManifest = Content,
																	  .S3TimeStamp = 0,
																	  .S3Size = 0,
																	  .Revision = Revision,
																	  .Image = Image,
																	  .Compatible = Compatible,
																	  .Timestamp = TimeStamp,
																	  .URI = ""});
							}
						}
					}
				} else if (FileName.getExtension() == "bin") {
					//  we must remove -upgrade, so
					const auto &ReleaseName =
						FileName.getBaseName().substr(0, FileName.getBaseName().size() - 8);
					auto It = Bucket.find(ReleaseName);
					auto S3TimeStamp = (uint64_t)(Object.GetLastModified().Millis() / 1000);
					uint64_t S3Size = Object.GetSize();
					std::string URI = URIBase + "/" + FileName.getFileName();
					if (It != Bucket.end()) {
						It->second.S3TimeStamp = S3TimeStamp;
						It->second.S3Size = S3Size;
						It->second.S3Name = ReleaseName;
						It->second.URI = URI;
					} else {

						Bucket.emplace(ReleaseName, S3BucketEntry{.Valid = false,
																  .S3Name = "",
																  .S3ContentManifest = "",
																  .S3TimeStamp = S3TimeStamp,
																  .S3Size = S3Size,
																  .Revision = "",
																  .Image = "",
																  .Compatible = "",
																  .Timestamp = 0,
																  .URI = URI});
					}
				} else {
					// std::cout << "Ignoring " << FileName.getFileName() << std::endl;
				}
			}

			isDone = !Outcome.GetResult().GetIsTruncated();
			if (!isDone) {
				// std::cout << "Going for next run..." << std::endl;
				// auto Token = Outcome.GetResult().GetContinuationToken();
				auto Token = Outcome.GetResult().GetNextContinuationToken();
				Request.SetContinuationToken(Token);
				// std::cout << "Continuation set..." << std::endl;
			}
		}

		// std::cout << "Count:" << Count << "  Runs:" << Runs << std::endl;
		if (!Outcome.IsSuccess()) {
			poco_error(Logger(), fmt::format("Run({},{}) Error while doing ListObjectsV2: {}, {}",
                                             Runs, Count,
											 std::string{Outcome.GetError().GetExceptionName()},
											 std::string{Outcome.GetError().GetMessage()}));
			return false;
		}
		return true;
	}

	void S3BucketEntry::Print() const {
		if (Valid) {
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
		for (const auto &[Name, Entry] : B) {
			std::cout << "Release:" << Name << std::endl;
			Entry.Print();
		}
	}

} // namespace OpenWifi
