//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#include "StorageService.h"

namespace OpenWifi {

    int Storage::Start() {
        std::lock_guard		Guard(Mutex_);

        Logger().setLevel(Poco::Message::PRIO_NOTICE);

        StorageClass::Start();

        Create_Tables();

        return 0;
    }

    void Storage::Stop() {
        std::lock_guard		Guard(Mutex_);
        Logger().notice("Stopping.");
        StorageClass::Stop();
    }

    std::string Storage::TrimRevision(const std::string &R) {
        std::string Result = R;
/*        if(R.size()>63)
            Result = R.substr(0,63);
        else
            Result = R;
*/
        return Result;
    }


}
// namespace