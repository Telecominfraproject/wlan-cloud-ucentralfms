//
// Created by stephane bourque on 2021-06-02.
//

#include "RESTAPI_newFirmwareAvailable.h"
#include "ManifestCreator.h"

void RESTAPI_newFirmwareAvailable::handleRequest(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) {

    if (!ContinueProcessing(Request, Response))
        return;

    uCentral::Auth::APIKeyEntry     AuthEntry;

    if (!IsValidAPIKey(Request, Response, AuthEntry))
        return;

    if(AuthEntry.Access != uCentral::Auth::ALL && AuthEntry.Access !=uCentral::Auth::NEWFIRMWARENOTIFIER) {
        UnAuthorized(Response);
        return;
    }

    ParseParameters(Request);
    if(Request.getMethod()==Poco::Net::HTTPRequest::HTTP_GET)
        DoGet(Request, Response);
    else
        BadRequest(Response);
}

void RESTAPI_newFirmwareAvailable::DoGet(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) {
    try {

        auto Op = GetParameter("operation","");

        if( Op != "notify" ) {
            BadRequest(Response);
            return;
        }

        uCentral::ManifestCreator::Update();

        Poco::JSON::Object  O;
        O.set("status", "updating manifest");
        ReturnObject(O, Response);
        return;
    } catch (const Poco::Exception &E) {
        Logger_.log(E);
    }
    BadRequest(Response);
}
