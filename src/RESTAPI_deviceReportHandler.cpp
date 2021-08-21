//
// Created by stephane bourque on 2021-07-19.
//

#include "RESTAPI_deviceReportHandler.h"
#include "StorageService.h"
#include "RESTAPI_FMSObjects.h"
#include "Poco/JSON/Object.h"
#include "Daemon.h"

namespace OpenWifi {
    void RESTAPI_deviceReportHandler::handleRequest(Poco::Net::HTTPServerRequest &Request,
                                                    Poco::Net::HTTPServerResponse &Response) {
        if (!ContinueProcessing(Request, Response))
            return;
        if (!IsAuthorized(Request, Response))
            return;
        if (Request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            DoGet(Request, Response);
        else
            BadRequest(Request, Response);
    }

    void RESTAPI_deviceReportHandler::DoGet(Poco::Net::HTTPServerRequest &Request,
                                            Poco::Net::HTTPServerResponse &Response) {
        try {
            Daemon()->CreateDashboard();
            Poco::JSON::Object  O;
            Daemon()->GetDashboard().to_json(O);
            ReturnObject(Request, O, Response);
            return;
        } catch ( const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }
}