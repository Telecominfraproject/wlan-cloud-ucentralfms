//
// Created by stephane bourque on 2021-07-16.
//

#include "RESTAPI_firmwareAgeHandler.h"

#include "StorageService.h"
#include "Poco/JSON/Parser.h"
#include "Daemon.h"
#include "Utils.h"

namespace uCentral {
    void RESTAPI_firmwareAgeHandler::handleRequest(Poco::Net::HTTPServerRequest &Request,
                                                Poco::Net::HTTPServerResponse &Response) {
        if (!ContinueProcessing(Request, Response))
            return;
        if (!IsAuthorized(Request, Response))
            return;
        ParseParameters(Request);
        if (Request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            DoGet(Request, Response);
        else
            BadRequest(Request, Response);
    }

    void
    RESTAPI_firmwareAgeHandler::DoGet(Poco::Net::HTTPServerRequest &Request, Poco::Net::HTTPServerResponse &Response) {
        try {
            auto DeviceType = GetParameter("deviceType","");
            auto Revision = GetParameter("revision","");

            if (DeviceType.empty() || Revision.empty()) {
                BadRequest(Request, Response, "Both deviceType and revision must be set.");
                return;
            }

            FMSObjects::FirmwareAgeDetails  FA;
            if(Storage()->ComputeFirmwareAge(DeviceType, Revision,FA)) {
                Poco::JSON::Object  Answer;

                FA.to_json(Answer);
                ReturnObject(Request, Answer, Response);
                return;
            } else {
                NotFound(Request, Response);
            }
            return;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }
}