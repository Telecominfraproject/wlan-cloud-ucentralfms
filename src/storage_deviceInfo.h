//
// Created by stephane bourque on 2021-07-16.
//

#ifndef UCENTRALFMS_STORAGE_DEVICEINFO_H
#define UCENTRALFMS_STORAGE_DEVICEINFO_H

#include "StorageService.h"

namespace uCentral {
    static const std::string DBNAME_DEVICES{"devices"};

    static const std::string DBFIELDS_DEVICES_CREATION {
            "serialNumber    varchar(36) UNIQUE PRIMARY KEY, "
            "revision           varchar, "
            "deviceType         varchar, "
            "endPoint           varchar, "
            "lastUpdate         bigint, "
            "status             varchar "
    };

    static const std::string DBFIELDS_DEVICES_SELECT{
            "serialNumber, "
            "revision, "
            "deviceType, "
            "endPoint, "
            "lastUpdate, "
            "status "
    };

    static const std::string DBFIELDS_DEVICES_UPDATE {
            "serialNumber=?, "
            "revision=?, "
            "deviceType=?, "
            "endPoint=?, "
            "lastUpdate=?, "
            "status=? "
    };

    typedef Poco::Tuple<
            std::string,
            std::string,
            std::string,
            std::string,
            uint64_t,
            std::string> DevicesRecord;
    typedef std::vector<DevicesRecord> DevicesRecordList;

}

#endif //UCENTRALFMS_STORAGE_DEVICEINFO_H
