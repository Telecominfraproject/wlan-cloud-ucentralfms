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
#include "Poco/JSON/Object.h"

#include "RESTAPI_FMSObjects.h"
#include "SubSystemServer.h"

#include "storage_firmwares.h"
#include "storage_history.h"
#include "storage_deviceTypes.h"

#ifndef SMALL_BUILD
#include "Poco/Data/PostgreSQL/Connector.h"
#include "Poco/Data/MySQL/Connector.h"
#include "Poco/Data/ODBC/Connector.h"
#endif

namespace uCentral {

    class Storage : public SubSystemServer {
    public:

        enum StorageType {
            sqlite,
            pgsql,
            mysql,
            odbc
        };

        Storage() noexcept;

        int Create_Tables();
        int Create_Firmwares();
        int Create_History();
        int Create_DeviceTypes();
        int Create_LatestFirmwareList();

        bool AddFirmware(FMSObjects::Firmware & F);
        bool UpdateFirmware(std::string & UUID, FMSObjects::Firmware & C);
        bool DeleteFirmware(std::string & UUID);
        bool GetFirmware(std::string & UUID, FMSObjects::Firmware & C);
        bool GetFirmwares(uint64_t From, uint64_t HowMany, std::vector<FMSObjects::Firmware> & Firmwares);
        bool BuildFirmwareManifest(Poco::JSON::Object & Manifest, uint64_t & Version);
        bool GetFirmwareByName(std::string & Release, std::string &DeviceType,FMSObjects::Firmware & C );
        uint64_t FirmwareVersion();

        bool GetHistory(std::string &SerialNumber,uint64_t From, uint64_t HowMany,FMSObjects::RevisionHistoryEntryVec &History);
        bool AddHistory(FMSObjects::RevisionHistoryEntry &History);

        void PopulateLatestFirmwareCache();

        int 	Start() override;
        void 	Stop() override;
        int 	Setup_SQLite();
        [[nodiscard]] std::string ConvertParams(const std::string &S) const;

        static Storage *instance() {
            if (instance_ == nullptr) {
                instance_ = new Storage;
            }
            return instance_;
        }

#ifndef SMALL_BUILD
        int 	Setup_MySQL();
        int 	Setup_PostgreSQL();
        int 	Setup_ODBC();
#endif

	  private:
		static Storage      							    *instance_;
		std::unique_ptr<Poco::Data::SessionPool>            Pool_= nullptr;
        StorageType 										dbType_ = sqlite;
        std::unique_ptr<Poco::Data::SQLite::Connector>  	SQLiteConn_= nullptr;
#ifndef SMALL_BUILD
        std::unique_ptr<Poco::Data::PostgreSQL::Connector>  PostgresConn_= nullptr;
        std::unique_ptr<Poco::Data::MySQL::Connector>       MySQLConn_= nullptr;
        std::unique_ptr<Poco::Data::ODBC::Connector>        ODBCConn_= nullptr;
#endif

   };

    inline Storage * Storage() { return Storage::instance(); };

}  // namespace

#endif //UCENTRAL_USTORAGESERVICE_H
