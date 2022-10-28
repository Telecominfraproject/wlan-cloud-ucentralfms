//
// Created by stephane bourque on 2021-05-09.
//

#include "Poco/JSON/Parser.h"

#include "RESTAPI/RESTAPI_firmwareHandler.h"
#include "StorageService.h"
#include "framework/ow_constants.h"
#include "framework/MicroServiceFuncs.h"
#include "framework/utils.h"

namespace OpenWifi {
    void
    RESTAPI_firmwareHandler::DoPost() {
        const auto &Obj = ParsedBody_;
        FMSObjects::Firmware F;
        if (!F.from_json(Obj)) {
            return BadRequest(RESTAPI::Errors::InvalidJSONDocument);
        }
        F.id = MicroServiceCreateUUID();
        if(StorageService()->FirmwaresDB().AddFirmware(F)) {
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
        if (StorageService()->FirmwaresDB().GetFirmware(UUID, F)) {
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

        if (StorageService()->FirmwaresDB().DeleteFirmware(UUID)) {
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
        if(!StorageService()->FirmwaresDB().GetFirmware(UUID, F)) {
            return NotFound();
        }

        const auto & Obj = ParsedBody_;
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
                SecurityObjects::NoteInfo   ii{.created=(uint64_t)Utils::Now(), .createdBy=UserInfo_.userinfo.email, .note=i.note};
                F.notes.push_back(ii);
            }
        }

        if(StorageService()->FirmwaresDB().UpdateFirmware(UUID, F)) {
            Poco::JSON::Object  Answer;
            F.to_json(Answer);
            return ReturnObject(Answer);
        }
        BadRequest(RESTAPI::Errors::RecordNotUpdated);
    }
}