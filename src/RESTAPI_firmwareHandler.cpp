//
// Created by stephane bourque on 2021-05-09.
//

#include "RESTAPI_firmwareHandler.h"
#include "StorageService.h"

#include "Poco/JSON/Parser.h"
#include "Daemon.h"

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
            auto UUID = GetBinding("id", "");

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
            auto UUID = GetBinding("id", "");

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
}