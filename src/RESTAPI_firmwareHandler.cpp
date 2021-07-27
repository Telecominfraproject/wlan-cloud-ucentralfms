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

namespace uCentral {
    void RESTAPI_firmwareHandler::handleRequest(Poco::Net::HTTPServerRequest &Request,
                                                Poco::Net::HTTPServerResponse &Response) {
        if (!ContinueProcessing(Request, Response))
            return;

        if (!IsAuthorized(Request, Response))
            return;

        ParseParameters(Request);

        if (Request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            DoGet(Request, Response);
        else if (Request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST)
            DoPost(Request, Response);
        else if (Request.getMethod() == Poco::Net::HTTPRequest::HTTP_PUT)
            DoPut(Request, Response);
        else if (Request.getMethod() == Poco::Net::HTTPRequest::HTTP_DELETE)
            DoDelete(Request, Response);
        else
            BadRequest(Request, Response);
    }

    void
    RESTAPI_firmwareHandler::DoPost(Poco::Net::HTTPServerRequest &Request, Poco::Net::HTTPServerResponse &Response) {
        try {
            Poco::JSON::Parser IncomingParser;
            Poco::JSON::Object::Ptr Obj =
                    IncomingParser.parse(Request.stream()).extract<Poco::JSON::Object::Ptr>();

            FMSObjects::Firmware F;
            if (!F.from_json(Obj)) {
                BadRequest(Request, Response);
                return;
            }
            F.id = Daemon()->CreateUUID();
            if(Storage()->AddFirmware(F)) {
                Poco::JSON::Object  Answer;
                F.to_json(Answer);
                ReturnObject(Request, Answer, Response);
                return;
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }

    void
    RESTAPI_firmwareHandler::DoGet(Poco::Net::HTTPServerRequest &Request, Poco::Net::HTTPServerResponse &Response) {
        try {
            auto UUID = GetBinding(uCentralProtocol::ID, "");

            if (!UUID.empty()) {
                FMSObjects::Firmware F;
                if (Storage()->GetFirmware(UUID, F)) {
                    Poco::JSON::Object Object;
                    F.to_json(Object);
                    ReturnObject(Request, Object, Response);
                } else {
                    NotFound(Request, Response);
                }
                return;
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }

    void
    RESTAPI_firmwareHandler::DoDelete(Poco::Net::HTTPServerRequest &Request, Poco::Net::HTTPServerResponse &Response) {
        try {
            auto UUID = GetBinding(uCentralProtocol::ID, "");

            if (!UUID.empty()) {
                if (Storage()->DeleteFirmware(UUID)) {
                    OK(Request, Response);
                } else {
                    NotFound(Request, Response);
                }
                return;
            }

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }

    void RESTAPI_firmwareHandler::DoPut(Poco::Net::HTTPServerRequest &Request,
                                        Poco::Net::HTTPServerResponse &Response) {
        try {

            auto UUID = GetBinding(uCentralProtocol::ID, "");
            if(UUID.empty()) {
                BadRequest(Request, Response, "UUID must be included in the request");
                return;
            }

            Poco::JSON::Parser IncomingParser;
            Poco::JSON::Object::Ptr Obj =
                    IncomingParser.parse(Request.stream()).extract<Poco::JSON::Object::Ptr>();

            FMSObjects::Firmware F;
            if(!Storage()->GetFirmware(UUID, F)) {
                NotFound(Request, Response);
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
                ReturnObject(Request, Answer, Response);
            } else {
                BadRequest(Request, Response, "Could not update the firmware entry. Please review your changes.");
            }
            return;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }
}