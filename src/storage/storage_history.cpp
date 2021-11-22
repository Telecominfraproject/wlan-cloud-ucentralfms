//
// Created by stephane bourque on 2021-07-12.
//

#include "StorageService.h"
#include "storage_history.h"
#include "RESTObjects/RESTAPI_FMSObjects.h"
#include "Daemon.h"

namespace OpenWifi {
/*
 *             " id, "
            "serialNumber, "
            "fromRelease, "
            "toRelease, "
            "commandUUID, "
            "revisionId, "
            "upgraded "

 */
    bool Convert(const HistoryRecord &T, FMSObjects::RevisionHistoryEntry & F) {
        F.id = T.get<0>();
        F.serialNumber = T.get<1>();
        F.fromRelease = T.get<2>();
        F.toRelease = T.get<3>();
        F.commandUUID = T.get<4>();
        F.revisionId = T.get<5>();
        F.upgraded = T.get<6>();
        return true;
    }

    bool Convert(const FMSObjects::RevisionHistoryEntry & F, HistoryRecord & T) {
        T.set<0>(F.id);
        T.set<1>(F.serialNumber);
        T.set<2>(F.fromRelease);
        T.set<3>(F.toRelease);
        T.set<4>(F.commandUUID);
        T.set<5>(F.revisionId);
        T.set<6>(F.upgraded);
        return true;
    }

    bool Storage::GetHistory(std::string &SerialNumber,uint64_t From, uint64_t HowMany,FMSObjects::RevisionHistoryEntryVec &History) {
        try {
            HistoryRecordList       Records;
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string st{"SELECT " + DBFIELDS_HISTORY_SELECT +
                           " FROM " + DBNAME_HISTORY + " where SerialNumber=? ORDER BY Upgraded DESC " };

            Select << ConvertParams(st) + ComputeRange(From, HowMany),
                    Poco::Data::Keywords::into(Records),
                    Poco::Data::Keywords::use(SerialNumber);
            Select.execute();

            for(const auto &R:Records) {
                FMSObjects::RevisionHistoryEntry   F;
                Convert(R,F);
                History.push_back(F);
            }
            return true;
        } catch(const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::AddHistory(FMSObjects::RevisionHistoryEntry &History) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Insert(Sess);

            std::string st{"INSERT INTO " + DBNAME_HISTORY + " (" + DBFIELDS_HISTORY_SELECT +
                    " ) values(?,?,?,?,?,?,?)" };

            std::cout << "Adding history for " << History.serialNumber << std::endl;
            HistoryRecordList   RL;
            HistoryRecord       R;
            Convert(History, R);
            RL.push_back(R);

            Insert <<   ConvertParams(st),
                        Poco::Data::Keywords::use(RL);
            Insert.execute();
            return true;
        } catch(const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::AddHistory( std::string & SerialNumber, std::string &DeviceType, std::string & PreviousRevision, std::string & NewVersion) {
            FMSObjects::RevisionHistoryEntry    History{
                .id = MicroService::CreateUUID(),
                .serialNumber = SerialNumber,
                .fromRelease = PreviousRevision,
                .toRelease = NewVersion,
                .upgraded = (uint64_t)std::time(nullptr)};

            FMSObjects::Firmware    F;
            if(GetFirmwareByRevision(NewVersion,DeviceType,F)) {
                History.revisionId = F.id;
            } else {
                History.revisionId = "unknown";
            }

            return AddHistory(History);
    }

    bool Storage::DeleteHistory( std::string & SerialNumber, std::string &Id) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Delete(Sess);

            std::string st{"DELETE FROM " + DBNAME_HISTORY + " where id=? and serialnumber=?"};

            Delete <<   ConvertParams(st),
                        Poco::Data::Keywords::use(Id),
                        Poco::Data::Keywords::use(SerialNumber);
            Delete.execute();
            return true;
        } catch(const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::DeleteHistory( std::string & SerialNumber) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Delete(Sess);

            std::string st{"DELETE FROM " + DBNAME_HISTORY + " where serialnumber=?"};

            Delete <<   ConvertParams(st),
            Poco::Data::Keywords::use(SerialNumber);
            Delete.execute();
            return true;
        } catch(const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }


}