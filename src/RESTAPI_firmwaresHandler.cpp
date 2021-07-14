//
// Created by stephane bourque on 2021-05-09.
//

#include "RESTAPI_firmwaresHandler.h"
#include "StorageService.h"
#include "LatestFirmwareCache.h"

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
            std::string DeviceType = GetParameter("deviceType","");
            bool IdOnly = GetBoolParameter("idOnly",false);
            bool RevisionSet = GetBoolParameter("revisionSet",false);
            bool LatestOnly = GetBoolParameter("latestOnly",false);

            std::cout << "I:" << IdOnly << " R:" << RevisionSet << " L:" << LatestOnly << std::endl;

            if(RevisionSet) {
                auto Revisions = LatestFirmwareCache()->GetRevisions();
                Poco::JSON::Array ObjectArray;
                for (const auto &i:Revisions) {
                    ObjectArray.add(i);
                }
                Poco::JSON::Object RetObj;
                RetObj.set("revisions", ObjectArray);
                ReturnObject(Request, RetObj, Response);
                return;
            }

            // special cases: if latestOnly and deviceType
            if(HasParameter("deviceType")) {
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
                        RetObj.set("firmwares", ObjectArray);
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
                RetObj.set("firmwares", ObjectArray);
                ReturnObject(Request, RetObj, Response);
                return;
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }
}
