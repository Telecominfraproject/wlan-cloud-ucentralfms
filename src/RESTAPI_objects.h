//
// Created by stephane bourque on 2021-05-07.
//

#ifndef UCENTRALFWS_RESTAPI_OBJECTS_H
#define UCENTRALFWS_RESTAPI_OBJECTS_H

#include "Poco/JSON/Object.h"

namespace uCentral::Objects {

    struct AclTemplate {
        bool Read_ = true ;
        bool ReadWrite_ = true ;
        bool ReadWriteCreate_ = true ;
        bool Delete_ = true ;
        bool PortalLogin_ = true ;
        void to_json(Poco::JSON::Object &Obj) const ;
    };

    struct WebToken {
        std::string access_token_;
        std::string refresh_token_;
        std::string id_token_;
        std::string token_type_;
        std::string username_;
        unsigned int expires_in_;
        unsigned int idle_timeout_;
        AclTemplate acl_template_;
        uint64_t    created_;
        void to_json(Poco::JSON::Object &Obj) const ;
    };

    struct Firmware {
        std::string UUID;
        std::string Description;
        std::string Owner;
        std::string Location;
        std::string DeviceType;
        std::string Uploader;
        std::string Digest;
        std::string FirmwareFileName;
        std::string FirmwareVersion;
        std::string FirmwareHash;
        std::string FirmwareLatestDoc;
        std::string S3URI;
        uint64_t    FirmwareDate;
        uint64_t    Uploaded;
        uint64_t    DownloadCount;
        uint64_t    Size;
        void to_json(Poco::JSON::Object &Obj) const;
    };

    struct Callback {
        std::string UUID;
        std::string URI;
        std::string Location;
        std::string Token;
        std::string TokenType;
        std::string Creator;
        uint64_t    Created;
        uint64_t    Expires;
        void to_json(Poco::JSON::Object &Obj) const;
        bool from_json(Poco::JSON::Object::Ptr Obj);
    };

    struct LatestFirmware {
        std::string DeviceType;
        std::string UUID;
        uint64_t    LastUpdated;
        void to_json(Poco::JSON::Object &Obj) const;
    };

}

#endif //UCENTRALFWS_RESTAPI_OBJECTS_H
