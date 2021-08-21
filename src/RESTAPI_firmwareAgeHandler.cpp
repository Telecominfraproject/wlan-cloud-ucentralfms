//
// Created by stephane bourque on 2021-07-16.
//

#include "RESTAPI_firmwareAgeHandler.h"

#include "StorageService.h"
#include "Poco/JSON/Parser.h"
#include "Daemon.h"
#include "Utils.h"
#include "DeviceCache.h"
#include "uCentralProtocol.h"
#include "RESTAPI_protocol.h"

namespace OpenWifi {
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

    void RESTAPI_firmwareAgeHandler::DoGet(Poco::Net::HTTPServerRequest &Request, Poco::Net::HTTPServerResponse &Response) {
        try {
            InitQueryBlock();
            if (!QB_.Select.empty()) {
                Poco::JSON::Array Objects;
                std::vector<std::string> Numbers = Utils::Split(QB_.Select);
                for (auto &i : Numbers) {
                    DeviceCacheEntry E;
                    if (DeviceCache()->GetDevice(i, E)) {
                        FMSObjects::FirmwareAgeDetails FA;
                        if(Storage()->ComputeFirmwareAge(E.deviceType,E.revision,FA)) {
                            Poco::JSON::Object  O;
                            FA.to_json(O);
                            O.set(uCentralProtocol::SERIALNUMBER,i);
                            Objects.add(O);
                        } else {
                            Poco::JSON::Object  O;
                            O.set(uCentralProtocol::SERIALNUMBER,i);
                            Objects.add(O);
                        }
                    } else {
                        Poco::JSON::Object  O;
                        O.set(uCentralProtocol::SERIALNUMBER,i);
                        Objects.add(O);
                    }
                }
                Poco::JSON::Object Answer;
                Answer.set(RESTAPI::Protocol::AGES, Objects);
                ReturnObject(Request, Answer, Response);
                return;
            } else {
                auto DeviceType = GetParameter(RESTAPI::Protocol::DEVICETYPE, "");
                auto Revision = GetParameter(RESTAPI::Protocol::REVISION, "");

                if (DeviceType.empty() || Revision.empty()) {
                    BadRequest(Request, Response, "Both deviceType and revision must be set.");
                    return;
                }

                Revision = Storage::TrimRevision(Revision);

                FMSObjects::FirmwareAgeDetails FA;
                if (Storage()->ComputeFirmwareAge(DeviceType, Revision, FA)) {
                    Poco::JSON::Object Answer;

                    FA.to_json(Answer);
                    ReturnObject(Request, Answer, Response);
                    return;
                } else {
                    NotFound(Request, Response);
                }
            }
            return;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }
}