//
// Created by stephane bourque on 2021-05-09.
//

#include "Poco/JSON/Parser.h"

#include "RESTAPI_firmwareHandler.h"
#include "StorageService.h"
#include "framework/uCentral_Protocol.h"
#include "framework/RESTAPI_protocol.h"
#include "framework/RESTAPI_errors.h"

namespace OpenWifi {
    void
    RESTAPI_firmwareHandler::DoPost() {
        auto Obj = ParseStream();
        FMSObjects::Firmware F;
        if (!F.from_json(Obj)) {
            return BadRequest(RESTAPI::Errors::InvalidJSONDocument);
        }
        F.id = MicroService::instance().CreateUUID();
        if(StorageService()->AddFirmware(F)) {
            Poco::JSON::Object  Answer;
            F.to_json(Answer);
            return ReturnObject(Answer);
        }
        BadRequest(RESTAPI::Errors::RecordNotCreated);
    }

    void
    RESTAPI_firmwareHandler::DoGet() {
        auto UUID = GetBinding(uCentralProtocol::ID, "");

        if(UUID.empty()) {
            return BadRequest(RESTAPI::Errors::MissingUUID);
        }

        FMSObjects::Firmware F;
        if (StorageService()->GetFirmware(UUID, F)) {
            Poco::JSON::Object Object;
            F.to_json(Object);
            return ReturnObject(Object);
        }
        NotFound();
    }

    void
    RESTAPI_firmwareHandler::DoDelete() {
        auto UUID = GetBinding(uCentralProtocol::ID, "");
        if(UUID.empty()) {
            return BadRequest(RESTAPI::Errors::MissingUUID);
        }

        if (StorageService()->DeleteFirmware(UUID)) {
            return OK();
        }
        BadRequest(RESTAPI::Errors::CouldNotBeDeleted);
    }

    void RESTAPI_firmwareHandler::DoPut() {
        auto UUID = GetBinding(uCentralProtocol::ID, "");
        if(UUID.empty()) {
            return BadRequest(RESTAPI::Errors::MissingUUID);
        }

        FMSObjects::Firmware    F;
        if(!StorageService()->GetFirmware(UUID, F)) {
            return NotFound();
        }

        auto Obj = ParseStream();
        FMSObjects::Firmware    NewFirmware;
        if(!NewFirmware.from_json(Obj)) {
            return BadRequest(RESTAPI::Errors::InvalidJSONDocument);
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

        if(StorageService()->UpdateFirmware(UUID, F)) {
            Poco::JSON::Object  Answer;
            F.to_json(Answer);
            return ReturnObject(Answer);
        }
        BadRequest(RESTAPI::Errors::RecordNotUpdated);
    }
}