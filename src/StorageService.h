//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#ifndef UCENTRAL_USTORAGESERVICE_H
#define UCENTRAL_USTORAGESERVICE_H

#include "framework/MicroService.h"
#include "framework/StorageClass.h"

#include "RESTObjects/RESTAPI_FMSObjects.h"
#include "storage/storage_firmwares.h"
#include "storage/storage_history.h"
#include "storage/storage_deviceTypes.h"
#include "storage/storage_deviceInfo.h"

namespace OpenWifi {

    class Storage : public StorageClass {
    public:

        int Create_Tables();
        int Create_Firmwares();
        int Create_History();
        int Create_DeviceTypes();
        int Create_DeviceInfo();

        bool AddFirmware(FMSObjects::Firmware & F);
        bool UpdateFirmware(std::string & UUID, FMSObjects::Firmware & C);
        bool DeleteFirmware(std::string & UUID);
        bool GetFirmware(std::string & UUID, FMSObjects::Firmware & C);
        bool GetFirmwares(uint64_t From, uint64_t HowMany, std::string & Compatible, FMSObjects::FirmwareVec & Firmwares);
        bool BuildFirmwareManifest(Poco::JSON::Object & Manifest, uint64_t & Version);
        bool GetFirmwareByName(std::string & Release, std::string &DeviceType,FMSObjects::Firmware & C );
        bool GetFirmwareByRevision(std::string & Revision, std::string &DeviceType,FMSObjects::Firmware & C );
        bool ComputeFirmwareAge(std::string & DeviceType, std::string & Revision, FMSObjects::FirmwareAgeDetails &AgeDetails);

        bool GetHistory(std::string &SerialNumber,uint64_t From, uint64_t HowMany,FMSObjects::RevisionHistoryEntryVec &History);
        bool AddHistory(FMSObjects::RevisionHistoryEntry &History);

        void PopulateLatestFirmwareCache();
        void RemoveOldFirmware();

        int 	Start() override;
        void 	Stop() override;

        bool SetDeviceRevision(std::string &SerialNumber, std::string & Revision, std::string & DeviceType, std::string &EndPoint);
        bool AddHistory( std::string & SerialNumber, std::string &DeviceType, std::string & PreviousRevision, std::string & NewVersion);
        bool DeleteHistory( std::string & SerialNumber, std::string &Id);

        bool GetDevices(uint64_t From, uint64_t HowMany, std::vector<FMSObjects::DeviceConnectionInformation> & Devices);
        bool GetDevice(std::string &SerialNumber, FMSObjects::DeviceConnectionInformation & Device);
        bool SetDeviceDisconnected(std::string &SerialNumber, std::string &EndPoint);

        bool GenerateDeviceReport(FMSObjects::DeviceReport &Report);
        static std::string TrimRevision(const std::string &R);
        static Storage *instance() {
            static Storage *instance_ = new Storage;
            return instance_;
        }

	  private:

    };

    inline class Storage * StorageService() { return Storage::instance(); };

}  // namespace

#endif //UCENTRAL_USTORAGESERVICE_H
