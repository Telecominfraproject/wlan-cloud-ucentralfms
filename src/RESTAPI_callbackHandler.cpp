//
// Created by stephane bourque on 2021-05-09.
//

#include "RESTAPI_callbackHandler.h"
#include "StorageService.h"
#include "Daemon.h"

#include "Poco/JSON/Parser.h"

void RESTAPI_callbackHandler::handleRequest(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) {
    if (!ContinueProcessing(Request, Response))
        return;

    if (!IsAuthorized(Request, Response))
        return;

    ParseParameters(Request);
    if(Request.getMethod()==Poco::Net::HTTPRequest::HTTP_GET)
        DoGet(Request, Response);
    else if (Request.getMethod()==Poco::Net::HTTPRequest::HTTP_POST)
        DoPost(Request, Response);
    else if (Request.getMethod()==Poco::Net::HTTPRequest::HTTP_PUT)
        DoPut(Request, Response);
    else if (Request.getMethod()==Poco::Net::HTTPRequest::HTTP_DELETE)
        DoDelete(Request, Response);
    else
        BadRequest(Response);
}

void RESTAPI_callbackHandler::DoPost(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) {
    try {
        Poco::JSON::Parser parser;
        Poco::JSON::Object::Ptr Obj = parser.parse(Request.stream()).extract<Poco::JSON::Object::Ptr>();

        uCentral::Objects::Callback C;

        if(C.from_json(Obj)) {
            C.UUID = uCentral::instance()->CreateUUID();

        }
    } catch (const Poco::Exception &E) {
        Logger_.log(E);
    }
    BadRequest(Response);
}

void RESTAPI_callbackHandler::DoPut(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) {
    try {

    } catch (const Poco::Exception &E) {
        Logger_.log(E);
    }
    BadRequest(Response);
}

void RESTAPI_callbackHandler::DoGet(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) {
    try {
        auto UUID = GetBinding("uuid","");

        if(!UUID.empty()) {
            uCentral::Objects::Callback C;
            if(uCentral::Storage::GetCallback(UUID,C))
            {
                Poco::JSON::Object  Object;

                C.to_json(Object);
                ReturnObject(Object,Response);
            } else {
                NotFound(Response);

            }
            return;
        }
    } catch (const Poco::Exception &E) {
        Logger_.log(E);
    }
    BadRequest(Response);
}

void RESTAPI_callbackHandler::DoDelete(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) {
    try {
        auto UUID = GetBinding("uuid","");

        if(!UUID.empty()) {
            if(uCentral::Storage::DeleteCallback(UUID))
            {
                OK(Response);
            } else {
                NotFound(Response);
            }
            return;
        }
    } catch (const Poco::Exception &E) {
        Logger_.log(E);
    }
    BadRequest(Response);
}
