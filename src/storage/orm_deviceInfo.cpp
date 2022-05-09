//
// Created by stephane bourque on 2021-12-29.
//

#include "orm_deviceInfo.h"
#include "StorageService.h"

/*
            "serialNumber    varchar(36) UNIQUE PRIMARY KEY, "
            "revision           varchar, "
            "deviceType         varchar, "
            "endPoint           varchar, "
            "lastUpdate         bigint, "
            "status             varchar "
 */

namespace OpenWifi {

    static ORM::FieldVec DevicesDB_Fields{
            ORM::Field{"serialNumber", 36, true},
            ORM::Field{"revision", ORM::FieldType::FT_TEXT},
            ORM::Field{"deviceType", ORM::FieldType::FT_TEXT},
            ORM::Field{"endPoint", ORM::FieldType::FT_TEXT},
            ORM::Field{"lastUpdate", ORM::FieldType::FT_BIGINT},
            ORM::Field{"status", ORM::FieldType::FT_TEXT}
    };

    DevicesDB::DevicesDB(OpenWifi::DBType T,
                         Poco::Data::SessionPool &P, Poco::Logger &L) :
            DB(T, "devices", DevicesDB_Fields,{}, P, L, "fdi") {
    }

    bool DevicesDB::SetDeviceRevision(std::string &SerialNumber, std::string & Revision, std::string & DeviceType, std::string &EndPoint) {
        FMSObjects::DeviceConnectionInformation D;
        if(GetRecord("serialNumber", SerialNumber, D)) {
            if(D.revision!=Revision) {
                StorageService()->HistoryDB().AddHistory(SerialNumber, DeviceType, D.revision, Revision);
            }
            D.revision = Revision;
            D.deviceType = DeviceType;
            D.endPoint = EndPoint;
            D.status = "connected";
            return UpdateRecord("serialNumber", SerialNumber, D);
        } else {
            FMSObjects::DeviceConnectionInformation   DI{
                    .serialNumber = SerialNumber,
                    .revision = Revision,
                    .deviceType = DeviceType,
                    .endPoint = EndPoint,
                    .lastUpdate = (uint64_t)OpenWifi::Now(),
                    .status = "connected"};
            return CreateRecord(DI);
        }
    }

    bool DevicesDB::DeleteDevice( std::string & SerialNumber) {
        return DeleteRecord("serialNumber", SerialNumber);
    }

    bool DevicesDB::SetDeviceDisconnected(std::string &SerialNumber, std::string &EndPoint) {
        FMSObjects::DeviceConnectionInformation D;
        if(GetRecord("serialNumber", SerialNumber, D)) {
            D.status = "not connected";
            D.endPoint = EndPoint;
            return UpdateRecord("serialNumber", SerialNumber, D);
        }
        return false;
    }

    bool DevicesDB::GetDevices(uint64_t From, uint64_t HowMany, std::vector<FMSObjects::DeviceConnectionInformation> & Devices) {
        return GetRecords(From, HowMany, Devices, "", " ORDER BY SerialNumber ASC ");
    }

    bool DevicesDB::GetDevice(std::string &SerialNumber, FMSObjects::DeviceConnectionInformation & Device) {
        return GetRecord("serialNumber", SerialNumber, Device);
    }

    bool DevicesDB::GenerateDeviceReport(FMSObjects::DeviceReport &Report) {
        Iterate([&Report](const FMSObjects::DeviceConnectionInformation &D) {
            Report.numberOfDevices++;
            UpdateCountedMap(Report.DeviceTypes_, D.deviceType);
            UpdateCountedMap(Report.Revisions_, D.revision);
            UpdateCountedMap(Report.Status_, D.status);
            UpdateCountedMap(Report.EndPoints_, D.endPoint);
            UpdateCountedMap(Report.OUI_, D.serialNumber.substr(0, 6));
            FMSObjects::FirmwareAgeDetails Age;
            if (StorageService()->FirmwaresDB().ComputeFirmwareAge(D.deviceType, D.revision, Age)) {
                if (Age.latest) {
                    UpdateCountedMap(Report.UsingLatest_, D.revision);
                } else if (Age.age == 0) {
                    UpdateCountedMap(Report.UnknownFirmwares_, D.revision);
                } else {
                    UpdateCountedMap(Report.totalSecondsOld_,"total_seconds", Age.age);
                }
            }
            return true;
        });
        return true;
    }

}
template<> void ORM::DB<OpenWifi::DevicesRecordTuple, OpenWifi::FMSObjects::DeviceConnectionInformation>::Convert(const OpenWifi::DevicesRecordTuple &T, OpenWifi::FMSObjects::DeviceConnectionInformation &F ) {
    F.serialNumber = T.get<0>();
    F.revision = T.get<1>();
    F.deviceType = T.get<2>();
    F.endPoint = T.get<3>();
    F.lastUpdate = T.get<4>();
    F.status = T.get<5>();
}

template<> void ORM::DB<OpenWifi::DevicesRecordTuple, OpenWifi::FMSObjects::DeviceConnectionInformation>::Convert(const OpenWifi::FMSObjects::DeviceConnectionInformation &F, OpenWifi::DevicesRecordTuple &T ) {
    T.set<0>(F.serialNumber);
    T.set<1>(F.revision);
    T.set<2>(F.deviceType);
    T.set<3>(F.endPoint);
    T.set<4>(F.lastUpdate);
    T.set<5>(F.status);
}

