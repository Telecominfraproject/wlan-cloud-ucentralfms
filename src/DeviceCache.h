//
// Created by stephane bourque on 2021-07-13.
//

#pragma once

#include "framework/SubSystemServer.h"
#include <string>

namespace OpenWifi {

	struct DeviceCacheEntry {
		std::string deviceType;
		std::string host;
		std::string revision;
	};
	typedef std::map<std::string, DeviceCacheEntry> DeviceCacheMap;

	class DeviceCache : public SubSystemServer {
	  public:
		static auto instance() {
			static auto instance_ = new DeviceCache;
			return instance_;
		}

		int Start() override;
		void Stop() override;
		void AddToCache(const std::string &serialNumber, const std::string &DeviceType,
						const std::string &Host, const std::string &Revision);
		void DumpCache();
		bool GetDevice(const std::string &SerialNumber, DeviceCacheEntry &E);

	  private:
		std::atomic_bool Running_ = false;
		DeviceCacheMap DeviceCache_;
		explicit DeviceCache() noexcept
			: SubSystemServer("DeviceCache", "DEVICE-CACHE", "devicecache") {}
	};

	inline auto DeviceCache() { return DeviceCache::instance(); }
} // namespace OpenWifi
