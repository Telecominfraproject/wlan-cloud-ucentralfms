//
// Created by stephane bourque on 2021-07-21.
//

#ifndef UCENTRALGW_DASHBOARD_H
#define UCENTRALGW_DASHBOARD_H

#include "framework/OpenWifiTypes.h"
#include "RESTObjects/RESTAPI_FMSObjects.h"

namespace OpenWifi {
	class DeviceDashboard {
	  public:
			void Create();
			const FMSObjects::DeviceReport & Report() const { return DB_;}
			inline void Reset() { LastRun_=0; DB_.reset(); }
	  private:
            FMSObjects::DeviceReport  	DB_;
			uint64_t 				LastRun_=0;
	};
}

#endif // UCENTRALGW_DASHBOARD_H
