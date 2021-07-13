//
// Created by stephane bourque on 2021-07-12.
//

#ifndef UCENTRALFMS_STORAGE_HISTORY_H
#define UCENTRALFMS_STORAGE_HISTORY_H

#include <string>
#include "Poco/Tuple.h"

namespace uCentral {
    static const std::string DBNAME_HISTORY{"history"};
    static const std::string DBFIELDS_HISTORY_CREATION {
        " id         varchar(36) UNIQUE PRIMARY KEY, "
        "serialNumber   varchar, "
        "fromRelease    varchar, "
        "toRelease      varchar, "
        "commandUUID    varchar, "
        "revisionId     varchar, "
        "upgraded       bigint "
    };

    static const std::string DBFIELDS_HISTORY_SELECT{
            " id, "
            "serialNumber, "
            "fromRelease, "
            "toRelease, "
            "commandUUID, "
            "revisionId, "
            "upgraded "
    };

    static const std::string DBFIELDS_HISTORY_UPDATE {
            " id=?, "
            "serialNumber=?, "
            "fromRelease=?, "
            "toRelease=?, "
            "commandUUID=?, "
            "revisionId=?, "
            "upgraded=? "
    };

    typedef Poco::Tuple<
        std::string,
        std::string,
        std::string,
        std::string,
        std::string,
        std::string,
        uint64_t> HistoryRecord;
    typedef std::vector<HistoryRecord> HistoryRecordList;

}


#endif //UCENTRALFMS_STORAGE_HISTORY_H
