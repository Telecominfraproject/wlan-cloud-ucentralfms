//
// Created by stephane bourque on 2021-06-02.
//

#pragma once

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/s3/S3Client.h>

#include "Poco/Timer.h"
#include "framework/SubSystemServer.h"

namespace OpenWifi {

	struct S3BucketEntry {
		bool Valid = false;
		std::string S3Name;
		std::string S3ContentManifest;
		uint64_t S3TimeStamp = 0;
		uint64_t S3Size = 0;
		std::string Revision;
		std::string Image;
		std::string Compatible;
		uint64_t Timestamp = 0;
		std::string URI;
		void Print() const;
	};
	typedef std::map<const std::string, S3BucketEntry> S3BucketContent;

	class ManifestCreator : public SubSystemServer, Poco::Runnable {
	  public:
		static auto instance() {
			static auto instance_ = new ManifestCreator;
			return instance_;
		}

		int Start() override;
		void Stop() override;

		bool ComputeManifest(S3BucketContent &BucketContent);
		bool AddManifestToDB(S3BucketContent &BucketContent);
		bool InitBucket();
		bool ReadBucket(S3BucketContent &Bucket);
		bool GetBucketObjectContent(Aws::S3::S3Client &S3Client, const std::string &ObjectName,
									std::string &ObjectContent);
		void CloseBucket();
		void Print(const S3BucketContent &B);
		uint64_t MaxAge() const { return MaxAge_; }
		void onTimer(Poco::Timer &timer);
		bool RunUpdateTask();
		void run() override;
		std::uint64_t LastUpdate() const { return LastUpdate_; }

	  private:
		std::atomic_bool Running_ = false;
		Aws::String S3BucketName_;
		Aws::String S3Region_;
		Aws::String S3Key_;
		Aws::String S3Secret_;
		uint64_t S3Retry_;
		Aws::Client::ClientConfiguration AwsConfig_{"ARILIA"};
		Aws::Auth::AWSCredentials AwsCreds_;
		uint64_t DBRefresh_ = 30 * 60;
		uint64_t MaxAge_ = 0;
		Poco::Timer Timer_;
		std::unique_ptr<Poco::TimerCallback<ManifestCreator>> ManifestCreatorCallBack_;
		std::atomic_flag UpdateRunning_ = ATOMIC_FLAG_INIT;
		Poco::Thread RunnerThread_;
		std::uint64_t LastUpdate_ = 0;

		ManifestCreator() noexcept
			: SubSystemServer("ManifestCreator", "MANIFEST-MGR", "manifestcreator") {}
	};

	inline auto ManifestCreator() { return ManifestCreator::instance(); };

} // namespace OpenWifi
