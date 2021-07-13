//
// Created by stephane bourque on 2021-07-12.
//

#include <string>

#ifndef UCENTRALFMS_RESTAPI_FMSOBJECTS_H
#define UCENTRALFMS_RESTAPI_FMSOBJECTS_H


#include "RESTAPI_SecurityObjects.h"

namespace uCentral::FMSObjects {

    struct Firmware {
        std::string     id;
        std::string     deviceType;
        std::string     description;
        std::string     revision;
        std::string     uri;
        std::string     image;
        uint64_t        imageDate;
        uint64_t        size;
        uint64_t        downloadCount;
        std::string     firmwareHash;
        std::string     owner;
        std::string     location;
        std::string     uploader;
        std::string     digest;
        bool            latest;
        SecurityObjects::NoteInfoVec    notes;
        uint64_t        created;

        void to_json(Poco::JSON::Object &Obj) const;
        bool from_json(const Poco::JSON::Object::Ptr &Obj);
    };
    typedef std::vector<Firmware>    FirmwareVec;

    struct FirmwareList {
        FirmwareVec  firmwares;

        void to_json(Poco::JSON::Object &Obj) const;
        bool from_json(const Poco::JSON::Object::Ptr &Obj);
    };

    struct DeviceType {
        std::string id;
        std::string deviceType;
        std::string manufacturer;
        std::string model;
        std::string policy;
        SecurityObjects::NoteInfoVec notes;
        uint64_t lastUpdate;
        uint64_t created;

        void to_json(Poco::JSON::Object &Obj) const;
        bool from_json(const Poco::JSON::Object::Ptr &Obj);
    };
    typedef std::vector<DeviceType> DeviceTypeVec;

    struct DeviceTypeList {
        DeviceTypeVec   deviceTypes;

        void to_json(Poco::JSON::Object &Obj) const;
        bool from_json(const Poco::JSON::Object::Ptr &Obj);
    };

    struct RevisionHistoryEntry {
        std::string id;
        std::string serialNumber;
        uint64_t upgraded;
        std::string commandUUID;

        void to_json(Poco::JSON::Object &Obj) const;
        bool from_json(const Poco::JSON::Object::Ptr &Obj);
    };
    typedef std::vector<RevisionHistoryEntry>   RevisionHistoryEntryVec;

    struct RevisionHistoryEntryList {
        RevisionHistoryEntryVec history;

        void to_json(Poco::JSON::Object &Obj) const;
        bool from_json(const Poco::JSON::Object::Ptr &Obj);
    };

}


#endif //UCENTRALFMS_RESTAPI_FMSOBJECTS_H
