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

namespace uCentral {

    class Storage : public SubSystemServer {

    public:
        Storage() noexcept;

        int Create_Tables();
        int Create_Firmwares();
        int Create_Callbacks();
        int Create_LatestFirmwareList();

        bool AddCallback(uCentral::Objects::Callback & C);
        bool AddOrUpdateCallback(uCentral::Objects::Callback & C);
        bool UpdateCallback(std::string & UUID, uCentral::Objects::Callback & C);
        bool DeleteCallback(std::string & UUID);
        bool GetCallback(std::string & UUID, uCentral::Objects::Callback & C);
        bool GetCallbacks(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Callback> & Callbacks);

        bool AddFirmware(uCentral::Objects::Firmware & F);
        bool UpdateFirmware(std::string & UUID, uCentral::Objects::Firmware & C);
        bool DeleteFirmware(std::string & UUID);
        bool GetFirmware(std::string & UUID, uCentral::Objects::Firmware & C);
        bool GetFirmwares(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Firmware> & Firmwares);
        bool BuildFirmwareManifest(Poco::JSON::Object & Manifest, uint64_t & Version);
        uint64_t FirmwareVersion();

        bool AddLatestFirmware(std::string & Compatible, std::string &UUID);
        bool GetLatestFirmware(std::string & Compatible, uCentral::Objects::LatestFirmware &L);
        bool DeleteLatestFirmware(std::string & Compatible);
        bool GetLatestFirmwareList(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::LatestFirmware> & LatestFirmwareList);

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

	  private:
		static Storage      							*instance_;
		std::unique_ptr<Poco::Data::SessionPool>        Pool_= nullptr;
		std::unique_ptr<Poco::Data::SQLite::Connector>  SQLiteConn_= nullptr;
		std::atomic_int64_t                             FirmwareVersion_ = 1 ;
   };

    inline Storage * Storage() { return Storage::instance(); };

}  // namespace

#endif //UCENTRAL_USTORAGESERVICE_H
