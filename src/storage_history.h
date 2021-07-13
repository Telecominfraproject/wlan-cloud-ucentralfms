//
// Created by stephane bourque on 2021-07-12.
//

#ifndef UCENTRALFMS_STORAGE_HISTORY_H
#define UCENTRALFMS_STORAGE_HISTORY_H

#include <string>

namespace uCentral {
    static const std::string DBNAME_HISTORY{"history"};
    static const std::string DBFIELDS_HISTORY_CREATION {
        " id          varchar(36) UNIQUE PRIMARY KEY, "
        "serialNumber   varchar, "
        "upgraded       bigint, "
        "commandUUID    varchar "
    };

    static const std::string DBFIELDS_HISTORY_SELECT{
        " id, "
        "serialNumber, "
        "upgraded, "
        "commandUUID "
    };

    static const std::string DBFIELDS_HISTORY_UPDATE {
        " id=?, "
        "serialNumber=?, "
        "upgraded=?, "
        "commandUUID=? "
    };
}


#endif //UCENTRALFMS_STORAGE_HISTORY_H
