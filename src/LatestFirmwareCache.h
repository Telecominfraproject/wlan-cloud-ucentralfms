//
// Created by stephane bourque on 2021-07-13.
//

#pragma once

#include "Poco/JSON/Object.h"
#include "Poco/JWT/Signer.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/SHA2Engine.h"
#include "Poco/StringTokenizer.h"

#include "RESTObjects/RESTAPI_SecurityObjects.h"
#include "framework/SubSystemServer.h"

namespace OpenWifi {

	struct LatestFirmwareCacheEntry {
		std::string Id;
		uint64_t TimeStamp = 0;
		std::string Revision;
	};
	typedef std::map<std::string, LatestFirmwareCacheEntry> LatestFirmwareCacheMap;
	typedef std::map<std::string, LatestFirmwareCacheEntry> rcOnlyLatestFirmwareCacheMap;

	class LatestFirmwareCache : public SubSystemServer {
	  public:
		static auto instance() {
			static auto instance_ = new LatestFirmwareCache;
			return instance_;
		}

		int Start() override;
		void Stop() override;
		bool AddToCache(const std::string &DeviceType, const std::string &Revision,
						const std::string &Id, uint64_t TimeStamp);
		// void AddRevision(const std::string &Revision);
		bool FindLatestFirmware(const std::string &DeviceType, LatestFirmwareCacheEntry &Entry);
		bool FindLatestRCOnlyFirmware(const std::string &DeviceType,
									  LatestFirmwareCacheEntry &Entry);

		inline static bool IsRC(const std::string &Revision) {
			// OpenWrt 21.02-SNAPSHOT r16399+120-c67509efd7 / TIP-v2.5.0-36b5478
			auto Tokens = Poco::StringTokenizer(Revision, "/", Poco::StringTokenizer::TOK_TRIM);
			if (Tokens.count() != 2)
				return false;
			return (Tokens[1].substr(0, 5) == "TIP-v");
		}

		void DumpCache();
		inline Types::StringSet GetRevisions() {
			std::lock_guard G(Mutex_);
			return RevisionSet_;
		};
		inline Types::StringSet GetDevices() {
			std::lock_guard G(Mutex_);
			return DeviceSet_;
		};
		bool IsLatest(const std::string &DeviceType, const std::string &Revision);
		bool IsLatestRCOnly(const std::string &DeviceType, const std::string &Revision);

	  private:
		LatestFirmwareCacheMap Cache_;
		rcOnlyLatestFirmwareCacheMap rcCache_;
		Types::StringSet RevisionSet_;
		Types::StringSet DeviceSet_;
		explicit LatestFirmwareCache() noexcept
			: SubSystemServer("LatestFirmwareCache", "LATEST-FIRMWARE-CACHE",
							  "LatestFirmwareCache") {}
	};

	inline auto LatestFirmwareCache() { return LatestFirmwareCache::instance(); }
} // namespace OpenWifi
