//
// Created by stephane bourque on 2021-07-18.
//

#include "RESTAPI_connectedDeviceHandler.h"
#include "RESTAPI_FMSObjects.h"
#include "StorageService.h"
#include "RESTAPI_protocol.h"

namespace OpenWifi {
    void RESTAPI_connectedDeviceHandler::handleRequest(Poco::Net::HTTPServerRequest &Request,
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

    void RESTAPI_connectedDeviceHandler::DoGet(Poco::Net::HTTPServerRequest &Request,
                                                Poco::Net::HTTPServerResponse &Response) {
        try {
            auto SerialNumber = GetBinding(RESTAPI::Protocol::SERIALNUMBER,"");

            if(SerialNumber.empty()) {
                BadRequest(Request, Response, "SerialNumber must be specified.");
                return;
            }

            FMSObjects::DeviceConnectionInformation DevInfo;
            if(Storage()->GetDevice(SerialNumber, DevInfo)) {
                Poco::JSON::Object  Answer;
                DevInfo.to_json(Answer);
                ReturnObject(Request, Answer, Response);
                return;
            }
            NotFound(Request, Response);
            return;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }

}