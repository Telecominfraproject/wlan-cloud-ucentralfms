//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#include "StorageService.h"

namespace OpenWifi {

	int Storage::Create_Tables() {
        Create_DeviceInfo();
		return 0;
	}

	int Storage::Create_DeviceInfo() {
        try {
            Poco::Data::Session Sess = Pool_->get();

            Sess << "CREATE TABLE IF NOT EXISTS " + DBNAME_DEVICES + " (" +
                    DBFIELDS_DEVICES_CREATION +
                    ")",
                    Poco::Data::Keywords::now;
            return 0;
        } catch(const Poco::Exception &E) {
            Logger().log(E);
        }

	    return -1;
	}


}

