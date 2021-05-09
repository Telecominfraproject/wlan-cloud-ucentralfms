//
// Created by stephane bourque on 2021-05-09.
//

#include "RESTAPI_firmwaresHandler.h"

void RESTAPI_firmwaresHandler::handleRequest(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) {
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

void RESTAPI_firmwaresHandler::DoGet(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {

}
