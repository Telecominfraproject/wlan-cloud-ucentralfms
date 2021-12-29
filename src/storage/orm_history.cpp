//
// Created by stephane bourque on 2021-12-28.
//

#include "orm_history.h"
#include "StorageService.h"

/*
        "id         varchar(36) UNIQUE PRIMARY KEY, "
        "serialNumber   varchar, "
        "fromRelease    varchar, "
        "toRelease      varchar, "
        "commandUUID    varchar, "
        "revisionId     varchar, "
        "upgraded       bigint "
 */


namespace OpenWifi {

    static ORM::FieldVec HistoryDB_Fields{
            ORM::Field{"id", 36, true},
            ORM::Field{"serialNumber", ORM::FieldType::FT_TEXT},
            ORM::Field{"fromRelease", ORM::FieldType::FT_TEXT},
            ORM::Field{"toRelease", ORM::FieldType::FT_TEXT},
            ORM::Field{"commandUUID", ORM::FieldType::FT_TEXT},
            ORM::Field{"revisionId", ORM::FieldType::FT_TEXT},
            ORM::Field{"upgraded", ORM::FieldType::FT_BIGINT}
    };

    HistoryDB::HistoryDB(OpenWifi::DBType T,
                       Poco::Data::SessionPool &P, Poco::Logger &L) :
            DB(T, "history", HistoryDB_Fields,{}, P, L, "his") {
    }

    bool HistoryDB::GetHistory(const std::string &SerialNumber, uint64_t From, uint64_t HowMany,
                             FMSObjects::RevisionHistoryEntryVec &History) {
        std::string WhereClause{" serialNumber='" + SerialNumber + "' "};
        std::string OrderBy{ " order by upgraded DESC "};
        return GetRecords(From, HowMany, History, WhereClause, OrderBy);
    }

    bool HistoryDB::AddHistory(FMSObjects::RevisionHistoryEntry &History) {
        return CreateRecord(History);
    }

    bool HistoryDB::AddHistory(std::string &SerialNumber, std::string &DeviceType, std::string &PreviousRevision,
                             std::string &NewVersion) {

        FMSObjects::RevisionHistoryEntry History{
                .id = MicroService::CreateUUID(),
                .serialNumber = SerialNumber,
                .fromRelease = PreviousRevision,
                .toRelease = NewVersion,
                .upgraded = (uint64_t) std::time(nullptr)};

        FMSObjects::Firmware F;
        if (StorageService()->FirmwaresDB().GetFirmwareByRevision(NewVersion, DeviceType, F)) {
            History.revisionId = F.id;
        } else {
            History.revisionId = "unknown";
        }

        return AddHistory(History);
    }

    bool HistoryDB::DeleteHistory(std::string &SerialNumber, std::string &Id) {
        return DeleteRecord("id", Id);
    }

    bool HistoryDB::DeleteHistory(std::string &SerialNumber) {
        std::string WhereClause{" serialNumber='" + SerialNumber + "' "};
        return DeleteRecords(WhereClause);
    }

}

template<> void ORM::DB<OpenWifi::HistoryRecordTuple, OpenWifi::FMSObjects::RevisionHistoryEntry>::Convert(OpenWifi::HistoryRecordTuple &T, OpenWifi::FMSObjects::RevisionHistoryEntry &F ) {
    F.id = T.get<0>();
    F.serialNumber = T.get<1>();
    F.fromRelease = T.get<2>();
    F.toRelease = T.get<3>();
    F.commandUUID = T.get<4>();
    F.revisionId = T.get<5>();
    F.upgraded = T.get<6>();
}

template<> void ORM::DB<OpenWifi::HistoryRecordTuple, OpenWifi::FMSObjects::RevisionHistoryEntry>::Convert(OpenWifi::FMSObjects::RevisionHistoryEntry &F, OpenWifi::HistoryRecordTuple &T ) {
    T.set<0>(F.id);
    T.set<1>(F.serialNumber);
    T.set<2>(F.fromRelease);
    T.set<3>(F.toRelease);
    T.set<4>(F.commandUUID);
    T.set<5>(F.revisionId);
    T.set<6>(F.upgraded);
}

