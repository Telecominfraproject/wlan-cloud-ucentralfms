//
// Created by stephane bourque on 2021-06-02.
//

#include "RESTAPI_newFirmwareAvailable.h"
#include "ManifestCreator.h"

namespace uCentral {
    void RESTAPI_newFirmwareAvailable::handleRequest(Poco::Net::HTTPServerRequest &Request,
                                                     Poco::Net::HTTPServerResponse &Response) {

        if (!ContinueProcessing(Request, Response))
            return;

        if (!ValidateAPIKey(Request, Response)) {
            UnAuthorized(Request, Response);
            return;
        }

        ParseParameters(Request);
        if (Request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            DoGet(Request, Response);
        else
            BadRequest(Request, Response);
    }

    void RESTAPI_newFirmwareAvailable::DoGet(Poco::Net::HTTPServerRequest &Request,
                                             Poco::Net::HTTPServerResponse &Response) {
        try {

            auto Op = GetParameter("operation", "");

            if (Op != "notify") {
                BadRequest(Request, Response);
                return;
            }

            uCentral::ManifestCreator()->Update();

            Poco::JSON::Object O;
            O.set("status", "updating manifest");
            ReturnObject(Request, O, Response);
            return;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }
}