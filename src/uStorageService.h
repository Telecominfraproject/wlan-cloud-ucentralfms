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

    bool AddCallback(uCentral::Objects::Callback & C);
    bool UpdateCallback(std::string & UUID, uCentral::Objects::Callback & C);
    bool DeleteCallback(std::string & UUID);
    bool GetCallback(std::string & UUID, uCentral::Objects::Callback & C);
    bool GetCallBacks(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Callback> & Callbacks);

    bool AddFirmware(uCentral::Objects::Firmware & F);
    bool UpdateFirmware(std::string & UUID, uCentral::Objects::Firmware & C);
    bool DeleteFirmware(std::string & UUID);
    bool GetFirmware(std::string & UUID, uCentral::Objects::Firmware & C);
    bool GetFirmwares(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Firmware> & Firmwares);

    bool AddLatestFirmware(std::string & DeviceType, std::string &UUID);
    bool GetLatestFirmware(std::string & DeviceType, std::string &UUID);
    bool DeleteLatestFirmware(std::string & DeviceType);
    bool GetLatestFirmwareList(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::LatestFirmware> & LatestFirmwareList);


    class Service : public SubSystemServer {

    public:
        Service() noexcept;

        friend int uCentral::Storage::Start();
        friend void uCentral::Storage::Stop();

        friend bool AddCallback(uCentral::Objects::Callback & C);
        friend bool UpdateCallback(std::string & UUID, uCentral::Objects::Callback & C);
        friend bool DeleteCallback(std::string & UUID);
        friend bool GetCallback(std::string & UUID, uCentral::Objects::Callback & C);
        friend bool GetCallBacks(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Callback> & Callbacks);

        friend bool AddFirmware(uCentral::Objects::Firmware & F);
        friend bool UpdateFirmware(std::string & UUID, uCentral::Objects::Firmware & C);
        friend bool DeleteFirmware(std::string & UUID);
        friend bool GetFirmware(std::string & UUID, uCentral::Objects::Firmware & C);
        friend bool GetFirmwares(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Firmware> & Firmwares);

        friend bool AddLatestFirmware(std::string & DeviceType, std::string &UUID);
        friend bool GetLatestFirmware(std::string & DeviceType, std::string &UUID);
        friend bool DeleteLatestFirmware(std::string & DeviceType);
        friend bool GetLatestFirmwareList(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::LatestFirmware> & LatestFirmwareList);


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
        int Create_LatestFirmwareList();

        bool AddCallback(uCentral::Objects::Callback & C);
        bool UpdateCallback(std::string & UUID, uCentral::Objects::Callback & C);
        bool DeleteCallback(std::string & UUID);
        bool GetCallback(std::string & UUID, uCentral::Objects::Callback & C);
        bool GetCallBacks(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Callback> & Callbacks);

        bool AddFirmware(uCentral::Objects::Firmware & F);
        bool UpdateFirmware(std::string & UUID, uCentral::Objects::Firmware & C);
        bool DeleteFirmware(std::string & UUID);
        bool GetFirmware(std::string & UUID, uCentral::Objects::Firmware & C);
        bool GetFirmwares(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Firmware> & Firmwares);

        bool AddLatestFirmware(std::string & DeviceType, std::string &UUID);
        bool GetLatestFirmware(std::string & DeviceType, std::string &UUID);
        bool DeleteLatestFirmware(std::string & DeviceType);
        bool GetLatestFirmwareList(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::LatestFirmware> & LatestFirmwareList);

        int 	Start() override;
        void 	Stop() override;
        int 	Setup_SQLite();
		[[nodiscard]] std::string ConvertParams(const std::string &S) const;
   };

}  // namespace

#endif //UCENTRAL_USTORAGESERVICE_H
