//
// Created by stephane bourque on 2021-12-28.
//

#include "orm_history.h"
#include "StorageService.h"

#include "framework/MicroServiceFuncs.h"
#include "fmt/format.h"

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

    bool HistoryDB::AddHistory(const std::string &SerialNumber, const std::string &DeviceType, const std::string &PreviousRevision,
                             const std::string &NewVersion) {

        FMSObjects::RevisionHistoryEntry History{
                .id = MicroServiceCreateUUID(),
                .serialNumber = SerialNumber,
                .fromRelease = PreviousRevision,
                .toRelease = NewVersion,
                .commandUUID = "",
                .revisionId = "",
                .upgraded = OpenWifi::Now()};

        FMSObjects::Firmware F;
        if (StorageService()->FirmwaresDB().GetFirmwareByRevision(NewVersion, DeviceType, F)) {
            History.revisionId = F.id;
        } else {
            History.revisionId = "unknown";
        }
        return AddHistory(History);
    }

    bool HistoryDB::DeleteHistory([[maybe_unused]] const std::string &SerialNumber, const std::string &Id) {
        return DeleteRecord("id", Id);
    }

    bool HistoryDB::DeleteHistory(const std::string &SerialNumber) {
        std::string WhereClause{" serialNumber='" + SerialNumber + "' "};
        return DeleteRecords(WhereClause);
    }

    bool HistoryDB::GetUnknownDeviceFirmwares(uint64_t offset, uint64_t limit,
                                              std::vector<FMSObjects::DeviceCurrentInfo> &Devices) {

        typedef Poco::Tuple<std::string, std::string, uint64_t > RecInfo;
        std::vector<RecInfo>    RecList;

        try {
            Poco::Data::Session sql_session = Pool_.get();
            auto sql_query = fmt::format("select max(upgraded), serialnumber from history where revisionid='unknown' group by serialnumber order by serialnumber offset={} limit={};)", offset, limit);
            Poco::Data::Statement sql_statement(sql_session);

            sql_statement <<    sql_query,
                                Poco::Data::Keywords::into(RecList),
                                Poco::Data::Keywords::now;

            for(const auto &record:RecList) {
                FMSObjects::DeviceCurrentInfo entry{
                    .serialNumber = record.get<1>(),
                    .revision = "unknown",
                    .upgraded = record.get<2>()
                };
                Devices.emplace_back(entry);
            }

            return true;
        } catch (const Poco::Exception &E) {
            Logger().log(E);
        }
        return false;
    }

    bool HistoryDB::GetDeviceFirmwares(uint64_t offset, uint64_t limit,
                                              std::vector<FMSObjects::DeviceCurrentInfo> &Devices) {

        typedef Poco::Tuple<uint64_t, std::string, std::string> RecInfo;
        std::vector<RecInfo>    RecList;

        try {
            Poco::Data::Session sql_session = Pool_.get();
            auto sql_query = fmt::format("select max(upgraded), serialNumber, toRelease from history where "
                                         "revisionId!='unknown' group by serialNumber, toRelease order by serialNumber, "
                                         "toRelease offset={} limit={};)", offset, limit);
            Poco::Data::Statement sql_statement(sql_session);

            sql_statement <<    sql_query,
                    Poco::Data::Keywords::into(RecList),
                    Poco::Data::Keywords::now;

            for(const auto &record:RecList) {
                FMSObjects::DeviceCurrentInfo entry{
                        .serialNumber = record.get<1>(),
                        .revision = record.get<2>(),
                        .upgraded = record.get<0>()
                };
                Devices.emplace_back(entry);
            }

            return true;
        } catch (const Poco::Exception &E) {
            Logger().log(E);
        }
        return false;
    }
}

template<> void ORM::DB<OpenWifi::HistoryRecordTuple, OpenWifi::FMSObjects::RevisionHistoryEntry>::Convert(const OpenWifi::HistoryRecordTuple &T, OpenWifi::FMSObjects::RevisionHistoryEntry &F ) {
    F.id = T.get<0>();
    F.serialNumber = T.get<1>();
    F.fromRelease = T.get<2>();
    F.toRelease = T.get<3>();
    F.commandUUID = T.get<4>();
    F.revisionId = T.get<5>();
    F.upgraded = T.get<6>();
}

template<> void ORM::DB<OpenWifi::HistoryRecordTuple, OpenWifi::FMSObjects::RevisionHistoryEntry>::Convert(const OpenWifi::FMSObjects::RevisionHistoryEntry &F, OpenWifi::HistoryRecordTuple &T ) {
    T.set<0>(F.id);
    T.set<1>(F.serialNumber);
    T.set<2>(F.fromRelease);
    T.set<3>(F.toRelease);
    T.set<4>(F.commandUUID);
    T.set<5>(F.revisionId);
    T.set<6>(F.upgraded);
}

