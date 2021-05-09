//
// Created by stephane bourque on 2021-05-09.
//

#include "RESTAPI_callbackHandler.h"

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
    else if (Request.getMethod()==Poco::Net::HTTPRequest::HTTP_DELETE)
        DoDelete(Request, Response);
    else
        BadRequest(Response);
}

void RESTAPI_callbackHandler::DoPost(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {

}

void RESTAPI_callbackHandler::DoGet(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {

}

void RESTAPI_callbackHandler::DoDelete(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) {

}
