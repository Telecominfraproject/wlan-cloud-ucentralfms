//
// Created by stephane bourque on 2021-07-21.
//

#pragma once

#include <mutex>

#include "Poco/Logger.h"
#include "RESTObjects/RESTAPI_FMSObjects.h"
#include "framework/OpenWifiTypes.h"

namespace OpenWifi {
	class DeviceDashboard {
	  public:
		bool Get(FMSObjects::DeviceReport &D, Poco::Logger &Logger);

	  private:
		std::mutex DataMutex_;
		volatile std::atomic_bool GeneratingDashboard_ = false;
		volatile bool ValidDashboard_ = false;
		FMSObjects::DeviceReport DB_;
		uint64_t LastRun_ = 0;

		void Generate(FMSObjects::DeviceReport &D, Poco::Logger &Logger);
	};
} // namespace OpenWifi
