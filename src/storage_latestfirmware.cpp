//
// Created by stephane bourque on 2021-05-09.
//

#include "StorageService.h"

namespace uCentral {

    typedef Poco::Tuple<
            std::string,
            std::string,
            uint64_t> LatestFirmwareRecordTuple;
    typedef std::vector<LatestFirmwareRecordTuple>  LatestFirmwareRecordList;

    bool Storage::AddLatestFirmware(std::string & Compatible, std::string &UUID) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string TmpUUID;
            std::string st{"SELECT UUID From LatestFirmwares WHERE Compatible=?"};

            Select << ConvertParams(st) ,
                Poco::Data::Keywords::into(TmpUUID),
                Poco::Data::Keywords::use(Compatible);
            Select.execute();

            uint64_t    LastUpdated = time(nullptr);

            if(TmpUUID.empty()) {
                Poco::Data::Statement   Insert(Sess);
                std::string st1{"INSERT INTO LatestFirmwares (Compatible, UUID, LastUpdated) VALUES(?,?,?)"};
                Insert << ConvertParams(st1),
                    Poco::Data::Keywords::use(Compatible),
                    Poco::Data::Keywords::use(UUID),
                    Poco::Data::Keywords::use(LastUpdated);
                Insert.execute();
            } else {
                Poco::Data::Statement   Update(Sess);
                std::string st1{"UPDATE LatestFirmwares SET UUID=?, LastUpdated=? WHERE Compatible=?"};
                Update << ConvertParams(st1),
                        Poco::Data::Keywords::use(UUID),
                        Poco::Data::Keywords::use(LastUpdated),
                        Poco::Data::Keywords::use(Compatible);
                Update.execute();
            }
            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::GetLatestFirmware(std::string & Compatible, uCentral::Objects::LatestFirmware &L) {
        try {
            Poco::Data::Session Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string TmpUUID;
            std::string st{"SELECT Compatible, UUID, LastUpdated From LatestFirmwares WHERE Compatible=?"};

            Select << ConvertParams(st) ,
                    Poco::Data::Keywords::into(L.Compatible),
                    Poco::Data::Keywords::into(L.UUID),
                    Poco::Data::Keywords::into(L.LastUpdated),
                    Poco::Data::Keywords::use(Compatible);
            Select.execute();

            return !L.UUID.empty();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::DeleteLatestFirmware(std::string & Compatible) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Delete(Sess);

            std::string TmpUUID;
            std::string st{"DELETE From LatestFirmwares WHERE Compatible=?"};

            Delete <<   ConvertParams(st),
                        Poco::Data::Keywords::use(Compatible);
            Delete.execute();
            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::GetLatestFirmwareList(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::LatestFirmware> & LatestFirmwareList) {
        try {
            LatestFirmwareRecordList    Records;
            Poco::Data::Session         Sess = Pool_->get();
            Poco::Data::Statement       Select(Sess);

            std::string st{"SELECT Compatible, UUID FROM LatestFirmwares"};

            Select <<   ConvertParams(st),
                        Poco::Data::Keywords::into(Records),
                        Poco::Data::Keywords::range(From, From + HowMany);
            Select.execute();

            for(const auto &i:Records) {
                uCentral::Objects::LatestFirmware   L{
                    .Compatible = i.get<0>(),
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


