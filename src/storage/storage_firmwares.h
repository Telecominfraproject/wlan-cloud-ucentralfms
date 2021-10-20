//
// Created by stephane bourque on 2021-07-12.
//

#ifndef UCENTRALFMS_STORAGE_FIRMWARES_H
#define UCENTRALFMS_STORAGE_FIRMWARES_H

#include "Poco/Tuple.h"

namespace OpenWifi {
    static const std::string DBNAME_FIRMWARES{"firmwares"};
    static const std::string DBFIELDS_FIRMWARES_CREATION {
            "Id              varchar(36) UNIQUE PRIMARY KEY, "
            "release            varchar, "
            "deviceType         varchar, "
            "description        varchar, "
            "revision           varchar, "
            "uri                varchar, "
            "image              varchar, "
            "imageDate          bigint, "
            "size               bigint, "
            "downloadCount      bigint, "
            "firmwareHash       varchar, "
            "owner              varchar, "
            "location           varchar, "
            "uploader           varchar, "
            "digest             varchar, "
            "latest             int,  "
            "notes              text, "
            "created            bigint"
    };

    static const std::string DBFIELDS_FIRMWARES_SELECT{
            " Id, "
            "release, "
            "deviceType, "
            "description, "
            "revision, "
            "uri, "
            "image, "
            "imageDate, "
            "size, "
            "downloadCount, "
            "firmwareHash, "
            "owner, "
            "location, "
            "uploader, "
            "digest, "
            "latest, "
            "notes, "
            "created "
    };

    static const std::string DBFIELDS_FIRMWARES_UPDATE {
            " Id=?, "
            "release=?, "
            "deviceType=?, "
            "description=?, "
            "revision=?, "
            "uri=?, "
            "image=?, "
            "imageDate=?, "
            "size=?, "
            "downloadCount=?, "
            "firmwareHash=?, "
            "owner=?, "
            "location=?, "
            "uploader=?, "
            "digest=?, "
            "latest=?, "
            "notes=?, "
            "created=? "
    };

    typedef Poco::Tuple<
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            uint64_t,
            uint64_t,
            uint64_t,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            uint64_t,
            std::string,
            uint64_t> FirmwaresRecord;
    typedef std::vector<FirmwaresRecord> FirmwaresRecordList;

}
#endif //UCENTRALFMS_STORAGE_FIRMWARES_H
