//
// Created by stephane bourque on 2021-05-09.
//

#include "RESTAPI_firmwaresHandler.h"
#include "StorageService.h"
#include "LatestFirmwareCache.h"
#include "RESTAPI_protocol.h"

namespace uCentral {
    void RESTAPI_firmwaresHandler::handleRequest(Poco::Net::HTTPServerRequest &Request,
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
    RESTAPI_firmwaresHandler::DoGet(Poco::Net::HTTPServerRequest &Request, Poco::Net::HTTPServerResponse &Response) {
        try {

            InitQueryBlock();
            std::string DeviceType = GetParameter(RESTAPI::Protocol::DEVICETYPE, "");
            bool IdOnly = GetBoolParameter(RESTAPI::Protocol::IDONLY, false);
            bool RevisionSet = GetBoolParameter(RESTAPI::Protocol::REVISIONSET, false);
            bool LatestOnly = GetBoolParameter(RESTAPI::Protocol::LATESTONLY, false);
            bool DeviceSet = GetBoolParameter(RESTAPI::Protocol::DEVICESET, false);

            if(DeviceSet) {
                auto Revisions = LatestFirmwareCache()->GetDevices();
                Poco::JSON::Array ObjectArray;
                for (const auto &i:Revisions) {
                    ObjectArray.add(i);
                }
                Poco::JSON::Object RetObj;
                RetObj.set(RESTAPI::Protocol::DEVICETYPES, ObjectArray);
                ReturnObject(Request, RetObj, Response);
                return;
            }

            if(RevisionSet) {
                auto Revisions = LatestFirmwareCache()->GetRevisions();
                Poco::JSON::Array ObjectArray;
                for (const auto &i:Revisions) {
                    ObjectArray.add(i);
                }
                Poco::JSON::Object RetObj;
                RetObj.set(RESTAPI::Protocol::REVISIONS, ObjectArray);
                ReturnObject(Request, RetObj, Response);
                return;
            }

            // special cases: if latestOnly and deviceType
            if(!DeviceType.empty()) {
                if(LatestOnly) {
                    LatestFirmwareCacheEntry    Entry;
                    if(!LatestFirmwareCache()->FindLatestFirmware(DeviceType,Entry)) {
                        NotFound(Request, Response);
                        return;
                    }

                    FMSObjects::Firmware    F;
                    if(Storage()->GetFirmware(Entry.Id,F)) {
                        Poco::JSON::Object  Answer;
                        F.to_json(Answer);
                        ReturnObject(Request, Answer, Response);
                        return;
                    }
                    NotFound(Request, Response);
                    return;
                } else {
                    std::vector<FMSObjects::Firmware> List;
                    if (Storage()->GetFirmwares(QB_.Offset, QB_.Limit, DeviceType, List)) {
                        Poco::JSON::Array ObjectArray;
                        for (const auto &i:List) {
                            if(IdOnly) {
                                ObjectArray.add(i.id);
                            } else {
                                Poco::JSON::Object Obj;
                                i.to_json(Obj);
                                ObjectArray.add(Obj);
                            }
                        }
                        Poco::JSON::Object RetObj;
                        RetObj.set(RESTAPI::Protocol::FIRMWARES, ObjectArray);
                        ReturnObject(Request, RetObj, Response);
                        return;
                    } else {
                        NotFound(Request, Response);
                        return;
                    }
                }
            }
            std::vector<FMSObjects::Firmware> List;
            if (Storage()->GetFirmwares(QB_.Offset, QB_.Limit, DeviceType, List)) {
                Poco::JSON::Array ObjectArray;
                for (const auto &i:List) {
                    if(IdOnly) {
                        ObjectArray.add(i.id);
                    } else {
                        Poco::JSON::Object Obj;
                        i.to_json(Obj);
                        ObjectArray.add(Obj);
                    }
                }
                Poco::JSON::Object RetObj;
                RetObj.set(RESTAPI::Protocol::FIRMWARES, ObjectArray);
                ReturnObject(Request, RetObj, Response);
                return;
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }
}
