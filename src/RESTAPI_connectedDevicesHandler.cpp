//
// Created by stephane bourque on 2021-07-18.
//

#include "RESTAPI_connectedDevicesHandler.h"
#include "RESTAPI_FMSObjects.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Array.h"
#include "StorageService.h"
#include "RESTAPI_protocol.h"

namespace uCentral {
    void RESTAPI_connectedDevicesHandler::handleRequest(Poco::Net::HTTPServerRequest &Request,
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

    void RESTAPI_connectedDevicesHandler::DoGet(Poco::Net::HTTPServerRequest &Request,
                                                Poco::Net::HTTPServerResponse &Response) {
        try {
            InitQueryBlock();

            std::vector<FMSObjects::DeviceConnectionInformation> Devices;
            if (Storage()->GetDevices(QB_.Offset, QB_.Limit, Devices)) {
                Poco::JSON::Array AnswerArr;
                for (const auto &i:Devices) {
                    Poco::JSON::Object Obj;
                    i.to_json(Obj);
                    AnswerArr.add(Obj);
                }
                Poco::JSON::Object AnswerObj;
                AnswerObj.set(RESTAPI::Protocol::DEVICES, AnswerArr);
                ReturnObject(Request, AnswerObj, Response);
                return;
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }
}