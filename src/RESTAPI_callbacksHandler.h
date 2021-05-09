//
// Created by stephane bourque on 2021-05-09.
//

#ifndef UCENTRALFWS_RESTAPI_CALLBACKSHANDLER_H
#define UCENTRALFWS_RESTAPI_CALLBACKSHANDLER_H

#include "RESTAPI_handler.h"

class RESTAPI_callbacksHandler : public RESTAPIHandler
{
public:
    RESTAPI_callbacksHandler(const RESTAPIHandler::BindingMap & bindings,Poco::Logger & L)
            : RESTAPIHandler(bindings,L,
                             std::vector<std::string>
                                     {  Poco::Net::HTTPRequest::HTTP_GET,
                                        Poco::Net::HTTPRequest::HTTP_POST,
                                        Poco::Net::HTTPRequest::HTTP_PUT,
                                        Poco::Net::HTTPRequest::HTTP_DELETE,
                                        Poco::Net::HTTPRequest::HTTP_OPTIONS}) {}
    void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response) override;
};
#endif //UCENTRALFWS_RESTAPI_CALLBACKSHANDLER_H
