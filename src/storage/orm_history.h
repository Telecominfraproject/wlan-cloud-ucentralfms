//
// Created by stephane bourque on 2021-12-28.
//

#pragma once

#include "framework/orm.h"
#include "RESTObjects/RESTAPI_FMSObjects.h"

namespace OpenWifi {

    typedef Poco::Tuple<
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            uint64_t> HistoryRecordTuple;
    typedef std::vector<HistoryRecordTuple>     HistoryRecordTupleList;

    class HistoryDB : public ORM::DB<HistoryRecordTuple, FMSObjects::RevisionHistoryEntry> {
    public:
        HistoryDB(OpenWifi::DBType T, Poco::Data::SessionPool & P, Poco::Logger &L);
        virtual ~HistoryDB() {};

        bool GetHistory(const std::string &SerialNumber, uint64_t From, uint64_t HowMany,
                                   FMSObjects::RevisionHistoryEntryVec &History);
        bool AddHistory(FMSObjects::RevisionHistoryEntry &History);
        bool AddHistory(const std::string &SerialNumber, const std::string &DeviceType, const std::string &PreviousRevision,
                                   const std::string &NewVersion);
        bool DeleteHistory(const std::string &SerialNumber, const std::string &Id);
        bool DeleteHistory(const std::string &SerialNumber);

        bool GetUnknownDeviceFirmwares(uint64_t offset, uint64_t limit, std::vector<FMSObjects::DeviceCurrentInfo> & Devices);
        bool GetDeviceFirmwares(uint64_t offset, uint64_t limit, std::vector<FMSObjects::DeviceCurrentInfo> & Devices);

    private:
    };
}
