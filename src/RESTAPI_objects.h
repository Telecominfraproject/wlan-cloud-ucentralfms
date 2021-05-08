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
        uint64_t    Uploaded;
        uint64_t    FirmwareDate;
        std::string Owner;
        std::string Location;
        std::string DeviceTypes;
        uint64_t    DownloadCount;
        std::string Uploader;
        uint64_t    Size;
        std::string Digest;
        std::string S3URI;
        void to_json(Poco::JSON::Object &Obj);
    };

    struct Callback {
        std::string UUID;
        std::string Creator;
        std::string Location;
        std::string Token;
        std::string TokenType;
        uint64_t    Created;
        uint64_t    Expires;
        void to_json(Poco::JSON::Object &Obj);
    };
}

#endif //UCENTRALFWS_RESTAPI_OBJECTS_H
