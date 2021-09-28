//
// Created by stephane bourque on 2021-05-09.
//

#include "Poco/JSON/Parser.h"

#include "RESTAPI_firmwareHandler.h"
#include "StorageService.h"
#include "Daemon.h"
#include "uCentralProtocol.h"
#include "RESTAPI_protocol.h"
#include "RESTAPI_utils.h"
#include "RESTAPI_errors.h"

namespace OpenWifi {
    void
    RESTAPI_firmwareHandler::DoPost() {
        auto Obj = ParseStream();
        FMSObjects::Firmware F;
        if (!F.from_json(Obj)) {
            BadRequest(RESTAPI::Errors::InvalidJSONDocument);
            return;
        }
        F.id = Daemon()->CreateUUID();
        if(Storage()->AddFirmware(F)) {
            Poco::JSON::Object  Answer;
            F.to_json(Answer);
            ReturnObject(Answer);
            return;
        }
        BadRequest(RESTAPI::Errors::RecordNotCreated);
    }

    void
    RESTAPI_firmwareHandler::DoGet() {
        auto UUID = GetBinding(uCentralProtocol::ID, "");

        if(UUID.empty()) {
            BadRequest(RESTAPI::Errors::MissingUUID);
            return;
        }

        FMSObjects::Firmware F;
        if (Storage()->GetFirmware(UUID, F)) {
            Poco::JSON::Object Object;
            F.to_json(Object);
            ReturnObject(Object);
            return;
        }
        NotFound();
    }

    void
    RESTAPI_firmwareHandler::DoDelete() {
        auto UUID = GetBinding(uCentralProtocol::ID, "");
        if(UUID.empty()) {
            BadRequest(RESTAPI::Errors::MissingUUID);
            return;
        }

        if (Storage()->DeleteFirmware(UUID)) {
            OK();
            return;
        }
        BadRequest(RESTAPI::Errors::CouldNotBeDeleted);
    }

    void RESTAPI_firmwareHandler::DoPut() {
        auto UUID = GetBinding(uCentralProtocol::ID, "");
        if(UUID.empty()) {
            BadRequest(RESTAPI::Errors::MissingUUID);
            return;
        }

        FMSObjects::Firmware    F;
        if(!Storage()->GetFirmware(UUID, F)) {
            NotFound();
            return;
        }

        auto Obj = ParseStream();
        FMSObjects::Firmware    NewFirmware;
        if(!NewFirmware.from_json(Obj)) {
            BadRequest(RESTAPI::Errors::InvalidJSONDocument);
            return;
        }

        if(Obj->has(RESTAPI::Protocol::DESCRIPTION))
            F.description = Obj->get(RESTAPI::Protocol::DESCRIPTION).toString();
        if(Obj->has(RESTAPI::Protocol::NOTES)) {
            SecurityObjects::NoteInfoVec NIV;
            NIV = RESTAPI_utils::to_object_array<SecurityObjects::NoteInfo>(Obj->get(RESTAPI::Protocol::NOTES).toString());
            for(auto const &i:NIV) {
                SecurityObjects::NoteInfo   ii{.created=(uint64_t)std::time(nullptr), .createdBy=UserInfo_.userinfo.email, .note=i.note};
                F.notes.push_back(ii);
            }
        }

        if(Storage()->UpdateFirmware(UUID, F)) {
            Poco::JSON::Object  Answer;
            F.to_json(Answer);
            ReturnObject(Answer);
            return;
        }
        BadRequest(RESTAPI::Errors::RecordNotUpdated);
    }
}