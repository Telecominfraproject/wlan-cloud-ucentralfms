//
// Created by stephane bourque on 2021-07-12.
//

#ifndef UCENTRALFMS_STORAGE_DEVICETYPES_H
#define UCENTRALFMS_STORAGE_DEVICETYPES_H

#include <string>

namespace OpenWifi {
    static const std::string DBNAME_DEVICETYPES{"deviceTypes"};
    static const std::string DBFIELDS_DEVICETYPES_CREATION {
            " id         varchar(36) UNIQUE PRIMARY KEY, "
            "deviceType     varchar, "
            "manufacturer   varchar, "
            "model          varchar, "
            "policy         varchar, "
            "notes          varchar, "
            "lastUpdate     bigint, "
            "created        bigint "
    };

    static const std::string DBFIELDS_DEVICETYPES_SELECT{
            " id, "
            "deviceType, "
            "manufacturer, "
            "model, "
            "policy, "
            "notes, "
            "lastUpdate, "
            "created "
    };

    static const std::string DBFIELDS_DEVICETYPES_UPDATE {
            " id=?, "
            "deviceType=?, "
            "manufacturer=?, "
            "model=?, "
            "policy=?, "
            "notes=?, "
            "lastUpdate=?, "
            "created=? "
    };

}

#endif //UCENTRALFMS_STORAGE_DEVICETYPES_H
