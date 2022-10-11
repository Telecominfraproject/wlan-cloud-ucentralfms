//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#pragma once

#include "framework/StorageClass.h"

#include "RESTObjects/RESTAPI_FMSObjects.h"

#include "storage/orm_history.h"
#include "storage/orm_firmwares.h"
#include "storage/orm_deviceInfo.h"

namespace OpenWifi {

    class Storage : public StorageClass {
    public:

        int 	Start() override;
        void 	Stop() override;

        int Create_Tables();
        int Create_DeviceTypes();
        int Create_DeviceInfo();

        bool BuildFirmwareManifest(Poco::JSON::Object & Manifest, uint64_t & Version);

        static std::string TrimRevision(const std::string &R);
        static auto instance() {
            static auto instance_ = new Storage;
            return instance_;
        }

        OpenWifi::HistoryDB & HistoryDB() { return * HistoryDB_; }
        OpenWifi::FirmwaresDB & FirmwaresDB() { return * FirmwaresDB_; }
        OpenWifi::DevicesDB & DevicesDB() { return * DevicesDB_; }

	  private:

        std::unique_ptr<OpenWifi::HistoryDB>        HistoryDB_;
        std::unique_ptr<OpenWifi::FirmwaresDB>      FirmwaresDB_;
        std::unique_ptr<OpenWifi::DevicesDB>        DevicesDB_;
    };

    inline auto StorageService() { return Storage::instance(); };

}  // namespace

