//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#ifndef UCENTRAL_USTORAGESERVICE_H
#define UCENTRAL_USTORAGESERVICE_H

#include "Poco/Data/Session.h"
#include "Poco/Data/SessionPool.h"
#include "Poco/Data/SQLite/Connector.h"

#include "RESTAPI_objects.h"
#include "SubSystemServer.h"
#include "uAuthService.h"

namespace uCentral::Storage {

    int Start();
    void Stop();


    class Service : public SubSystemServer {

    public:
        Service() noexcept;

        friend int uCentral::Storage::Start();
        friend void uCentral::Storage::Stop();

        static Service *instance() {
            if (instance_ == nullptr) {
                instance_ = new Service;
            }
            return instance_;
        }

	  private:
		static Service      							*instance_;
		std::unique_ptr<Poco::Data::SessionPool>        Pool_= nullptr;
		std::unique_ptr<Poco::Data::SQLite::Connector>  SQLiteConn_= nullptr;

		int Create_Tables();
        int Create_Firmwares();
        int Create_Callbacks();

        int 	Start() override;
        void 	Stop() override;
        int 	Setup_SQLite();
		[[nodiscard]] std::string ConvertParams(const std::string &S) const;
   };

}  // namespace

#endif //UCENTRAL_USTORAGESERVICE_H
