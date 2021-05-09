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
}