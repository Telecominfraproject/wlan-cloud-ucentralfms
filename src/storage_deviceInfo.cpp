//
// Created by stephane bourque on 2021-07-16.
//

#include "storage_deviceInfo.h"
#include "StorageService.h"
/*
            "serialNumber=?, "
            "revision=?, "
            "deviceType=?, "
            "endPoint=?, "
            "lastUpdate=?, "
            "status=?
 */
namespace uCentral {

    bool Convert(const DevicesRecord &T, FMSObjects::DeviceConnectionInformation & F) {
        F.serialNumber = T.get<0>();
        F.revision = T.get<1>();
        F.deviceType = T.get<2>();
        F.endPoint = T.get<3>();
        F.lastUpdate = T.get<4>();
        F.status = T.get<5>();
        return true;
    }

    bool Convert(const FMSObjects::DeviceConnectionInformation & F, DevicesRecord & T) {
        T.set<0>(F.serialNumber);
        T.set<1>(F.revision);
        T.set<2>(F.deviceType);
        T.set<3>(F.endPoint);
        T.set<4>(F.lastUpdate);
        T.set<5>(F.status);
        return true;
    }

    bool Storage::SetDeviceRevision(std::string &SerialNumber, std::string & Revision, std::string & DeviceType, std::string &EndPoint) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Insert(Sess);

            DevicesRecordList   Records;
            std::string         PreviousVersion;
            bool                DeviceExists=false;
            try {
                Poco::Data::Statement   Select(Sess);

                std::string St{"select " + DBFIELDS_DEVICES_SELECT + " from " + DBNAME_DEVICES + " where serialNumber=?"};
                Select <<   ConvertParams(St) ,
                            Poco::Data::Keywords::into(Records),
                            Poco::Data::Keywords::use(SerialNumber);
                Select.execute();
                if(!Records.empty()) {
                    PreviousVersion = Records[0].get<1>();
                    DeviceExists = true;
                }
            } catch (const Poco::Exception &E) {

            }

            if(!DeviceExists) {
                std::string st{"INSERT INTO " + DBNAME_DEVICES + " (" +
                               DBFIELDS_DEVICES_SELECT +
                               ") VALUES(?,?,?,?,?,?)"};
                Logger_.information(Poco::format("New device '%s' connected", SerialNumber));
                FMSObjects::DeviceConnectionInformation   DI{
                        .serialNumber = SerialNumber,
                        .revision = Revision,
                        .deviceType = DeviceType,
                        .endPoint = EndPoint,
                        .lastUpdate = (uint64_t)std::time(nullptr),
                        .status = "connected"};

                DevicesRecordList   InsertRecords;
                DevicesRecord       R;
                Convert(DI, R);
                InsertRecords.push_back(R);
                Insert  <<  ConvertParams(st),
                        Poco::Data::Keywords::use(InsertRecords);
                Insert.execute();
            } else {
                Poco::Data::Statement   Update(Sess);
                uint64_t Now = (uint64_t)std::time(nullptr);

                // std::cout << "Updating device: " << SerialNumber << std::endl;
                std::string st{"UPDATE " + DBNAME_DEVICES + " set revision=?, lastUpdate=?, endpoint=?, status=? " + " where serialNumber=?"};
                Update <<   ConvertParams(st) ,
                            Poco::Data::Keywords::use(Revision),
                            Poco::Data::Keywords::use(Now),
                            Poco::Data::Keywords::use(EndPoint),
                            "connected",
                            Poco::Data::Keywords::use(SerialNumber);
                Update.execute();

                if(PreviousVersion!=Revision)
                    AddHistory(SerialNumber, DeviceType, PreviousVersion, Revision);
            }
            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;

    }

    bool Storage::SetDeviceDisconnected(std::string &SerialNumber, std::string &EndPoint) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Update(Sess);
            uint64_t Now = (uint64_t)std::time(nullptr);

            // std::cout << "Updating device: " << SerialNumber << std::endl;
            std::string st{"UPDATE " + DBNAME_DEVICES + " set lastUpdate=?, endpoint=?, status=? " + " where serialNumber=?"};
            Update <<   ConvertParams(st) ,
                    Poco::Data::Keywords::use(Now),
                    Poco::Data::Keywords::use(EndPoint),
                    "disconnected",
                    Poco::Data::Keywords::use(SerialNumber);
            Update.execute();
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }


    bool Storage::GetDevices(uint64_t From, uint64_t HowMany, std::vector<FMSObjects::DeviceConnectionInformation> & Devices) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            DevicesRecordList   Records;

            std::string St{"select " + DBFIELDS_DEVICES_SELECT + " from " + DBNAME_DEVICES};
            Select <<   ConvertParams(St) ,
                    Poco::Data::Keywords::into(Records),
                    Poco::Data::Keywords::range(From, From + HowMany);
            Select.execute();

            for(const auto &i:Records) {
                FMSObjects::DeviceConnectionInformation DI;
                Convert(i,DI);
                Devices.push_back(DI);
            }

            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::GetDevice(std::string &SerialNumber, FMSObjects::DeviceConnectionInformation & Device) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            DevicesRecordList   Records;
            std::string St{"select " + DBFIELDS_DEVICES_SELECT + " from " + DBNAME_DEVICES + " where serialNumber=?"};
            Select <<   ConvertParams(St) ,
                    Poco::Data::Keywords::into(Records),
                    Poco::Data::Keywords::use(SerialNumber);
            Select.execute();

            if(!Records.empty()) {
                Convert(Records[0],Device);
                return true;
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }
}