//
// Created by stephane bourque on 2021-07-12.
//

#include "StorageService.h"
#include "storage_history.h"
#include "RESTAPI_FMSObjects.h"
#include "Daemon.h"

namespace uCentral {
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
                           " FROM " + DBNAME_HISTORY + " where SerialNumber=?" };

            Select << ConvertParams(st),
                    Poco::Data::Keywords::into(Records),
                    Poco::Data::Keywords::use(SerialNumber),
                    Poco::Data::Keywords::range(From, From + HowMany);
            Select.execute();

            for(const auto &R:Records) {
                FMSObjects::RevisionHistoryEntry   F;
                Convert(R,F);
                History.push_back(F);
            }
            return true;
        } catch(const Poco::Exception &E) {

        }
        return false;
    }

    bool Storage::AddHistory(FMSObjects::RevisionHistoryEntry &History) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Insert(Sess);

            std::string st{"INSERT INTO " + DBNAME_HISTORY + " (" + DBFIELDS_HISTORY_SELECT +
                    " ) values(?,?,?,?,?,?,?)" };

            HistoryRecordList R{ HistoryRecord{}};
            Convert(History, R[0]);

            Insert << ConvertParams(st),
                    Poco::Data::Keywords::use(R);
            Insert.execute();
            return true;
        } catch(const Poco::Exception &E) {

        }
        return false;
    }

    bool Storage::AddHistory( std::string & SerialNumber, std::string & PreviousRevision, std::string & NewVersion) {
            FMSObjects::RevisionHistoryEntry    History{
                .id = Daemon()->CreateUUID(),
                .serialNumber = SerialNumber,
                .fromRelease = PreviousRevision,
                .toRelease = NewVersion,
                .upgraded = (uint64_t)std::time(nullptr)};
            return AddHistory(History);
    }

}