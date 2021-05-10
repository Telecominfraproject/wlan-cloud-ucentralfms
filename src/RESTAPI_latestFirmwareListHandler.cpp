//
// Created by stephane bourque on 2021-05-09.
//

#include "RESTAPI_latestFirmwareListHandler.h"
#include "uStorageService.h"

void RESTAPI_latestFirmwareListHandler::handleRequest(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) {
    if (!ContinueProcessing(Request, Response))
        return;

    if (!IsAuthorized(Request, Response))
        return;

    ParseParameters(Request);

    if(Request.getMethod()==Poco::Net::HTTPRequest::HTTP_GET)
        DoGet(Request, Response);
    else
        BadRequest(Response);
}

void RESTAPI_latestFirmwareListHandler::DoGet(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) {

    try {
        auto Offset = GetParameter("offset", 0);
        auto Limit = GetParameter("limit", 100);

        std::vector<uCentral::Objects::LatestFirmware> List;
        if (uCentral::Storage::GetLatestFirmwareList(Offset, Limit, List)) {

            Poco::JSON::Array   ObjectArray;

            for(const auto &i:List) {
                Poco::JSON::Object  Obj;
                i.to_json(Obj);
                ObjectArray.add(Obj);
            }
            Poco::JSON::Object  RetObj;
            RetObj.set("latestFirmwareList",ObjectArray);
            ReturnObject(RetObj,Response);
            return;
        }
    } catch(const Poco::Exception &E) {
        Logger_.log(E);
    }
    BadRequest(Response);
}
