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

namespace OpenWifi {
    void
    RESTAPI_firmwareHandler::DoPost() {
        try {
            auto Obj = ParseStream();
            FMSObjects::Firmware F;
            if (!F.from_json(Obj)) {
                BadRequest("Ill formed JSON Document.");
                return;
            }
            F.id = Daemon()->CreateUUID();
            if(Storage()->AddFirmware(F)) {
                Poco::JSON::Object  Answer;
                F.to_json(Answer);
                ReturnObject(Answer);
                return;
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest("Internal error.");
    }

    void
    RESTAPI_firmwareHandler::DoGet() {
        try {
            auto UUID = GetBinding(uCentralProtocol::ID, "");

            if (!UUID.empty()) {
                FMSObjects::Firmware F;
                if (Storage()->GetFirmware(UUID, F)) {
                    Poco::JSON::Object Object;
                    F.to_json(Object);
                    ReturnObject(Object);
                } else {
                    NotFound();
                }
                return;
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest("Internal error.");
    }

    void
    RESTAPI_firmwareHandler::DoDelete() {
        try {
            auto UUID = GetBinding(uCentralProtocol::ID, "");

            if (!UUID.empty()) {
                if (Storage()->DeleteFirmware(UUID)) {
                    OK();
                } else {
                    NotFound();
                }
                return;
            }

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest("Internal error.");
    }

    void RESTAPI_firmwareHandler::DoPut() {
        try {

            auto UUID = GetBinding(uCentralProtocol::ID, "");
            if(UUID.empty()) {
                BadRequest("UUID must be included in the request");
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
                BadRequest("Ill-formed JSON document.");
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
            } else {
                BadRequest("Could not update the firmware entry. Please review your changes.");
            }
            return;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest("Internal error.");
    }
}