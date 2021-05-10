//
// Created by stephane bourque on 2021-05-09.
//

#include "uStorageService.h"

namespace uCentral::Storage {

    bool AddLatestFirmware(std::string & DeviceType, std::string &UUID) {
        return Service::instance()->AddLatestFirmware(DeviceType, UUID);
    }

    bool GetLatestFirmware(std::string & DeviceType, uCentral::Objects::LatestFirmware &L) {
        return Service::instance()->GetLatestFirmware(DeviceType, L);
    }

    bool DeleteLatestFirmware(std::string & DeviceType) {
        return Service::instance()->DeleteLatestFirmware(DeviceType);
    }

    bool GetLatestFirmwareList(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::LatestFirmware> & LatestFirmwareList) {
        return Service::instance()->GetLatestFirmwareList(From, HowMany, LatestFirmwareList);
    }

    typedef Poco::Tuple<
            std::string,
            std::string,
            uint64_t> LatestFirmwareRecordTuple;
    typedef std::vector<LatestFirmwareRecordTuple>  LatestFirmwareRecordList;

    bool Service::AddLatestFirmware(std::string & DeviceType, std::string &UUID) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string TmpUUID;
            std::string st{"SELECT UUID From LatestFirmwares WHERE DeviceType=?"};

            Select << ConvertParams(st) ,
                Poco::Data::Keywords::into(TmpUUID),
                Poco::Data::Keywords::use(DeviceType);
            Select.execute();

            uint64_t    LastUpdated = time(nullptr);

            if(TmpUUID.empty()) {
                Poco::Data::Statement   Insert(Sess);
                std::string st1{"INSERT INTO LatestFirmwares (DeviceType, UUID, LastUpdated) VALUES(?,?,?)"};
                Insert << ConvertParams(st1),
                    Poco::Data::Keywords::use(DeviceType),
                    Poco::Data::Keywords::use(UUID),
                    Poco::Data::Keywords::use(LastUpdated);
                Insert.execute();
            } else {
                Poco::Data::Statement   Update(Sess);
                std::string st1{"UPDATE LatestFirmwares SET UUID=?, LastUpdated=? WHERE DeviceType=?"};
                Update << ConvertParams(st1),
                        Poco::Data::Keywords::use(UUID),
                        Poco::Data::Keywords::use(LastUpdated),
                        Poco::Data::Keywords::use(DeviceType);
                Update.execute();
            }
            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::GetLatestFirmware(std::string & DeviceType, uCentral::Objects::LatestFirmware &L) {
        try {
            Poco::Data::Session Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string TmpUUID;
            std::string st{"SELECT DeviceType, UUID, LastUpdated From LatestFirmwares WHERE DeviceType=?"};

            Select << ConvertParams(st) ,
                    Poco::Data::Keywords::into(L.DeviceType),
                    Poco::Data::Keywords::into(L.UUID),
                    Poco::Data::Keywords::into(L.LastUpdated),
                    Poco::Data::Keywords::use(DeviceType);
            Select.execute();

            return !L.UUID.empty();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::DeleteLatestFirmware(std::string & DeviceType) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Delete(Sess);

            std::string TmpUUID;
            std::string st{"DELETE From LatestFirmwares WHERE DeviceType=?"};

            Delete <<   ConvertParams(st),
                        Poco::Data::Keywords::use(DeviceType);
            Delete.execute();
            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::GetLatestFirmwareList(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::LatestFirmware> & LatestFirmwareList) {
        try {
            LatestFirmwareRecordList    Records;
            Poco::Data::Session         Sess = Pool_->get();
            Poco::Data::Statement       Select(Sess);

            std::string st{"SELECT DeviceType, UUID FROM LatestFirmwares"};

            Select <<   ConvertParams(st),
                        Poco::Data::Keywords::into(Records),
                        Poco::Data::Keywords::range(From, From + HowMany);
            Select.execute();

            for(const auto &i:Records) {
                uCentral::Objects::LatestFirmware   L{
                    .DeviceType = i.get<0>(),
                    .UUID = i.get<1>()
                };
                LatestFirmwareList.push_back(L);
            }
            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

}


