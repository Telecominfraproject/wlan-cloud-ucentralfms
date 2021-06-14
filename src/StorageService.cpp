//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#include <fstream>
#include "StorageService.h"
#include "Poco/Util/Application.h"
#include "Utils.h"

namespace uCentral {

    class Storage *Storage::instance_ = nullptr;

    Storage::Storage() noexcept:
            SubSystemServer("Storage", "STORAGE-SVR", "storage")
    {
    }

	std::string Storage::ConvertParams(const std::string & S) const {
		std::string R;

		R.reserve(S.size()*2+1);

		if(false) {
			auto Idx=1;
			for(auto const & i:S)
			{
				if(i=='?') {
					R += '$';
					R.append(std::to_string(Idx++));
				} else {
					R += i;
				}
			}
		} else {
			R = S;
		}
		return R;
	}

    int Storage::Start() {
		SubMutexGuard		Guard(Mutex_);
		Logger_.setLevel(Poco::Message::PRIO_NOTICE);
        Logger_.notice("Starting.");
        Setup_SQLite();
		Create_Tables();
		return 0;
    }

    void Storage::Stop() {
        SubMutexGuard		Guard(Mutex_);
        Logger_.notice("Stopping.");
    }
}
// namespace