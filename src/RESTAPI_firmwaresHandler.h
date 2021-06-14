//
// Created by stephane bourque on 2021-05-09.
//

#ifndef UCENTRALFWS_RESTAPI_FIRMWARESHANDLER_H
#define UCENTRALFWS_RESTAPI_FIRMWARESHANDLER_H

#include "RESTAPI_handler.h"

namespace uCentral {
    class RESTAPI_firmwaresHandler : public RESTAPIHandler {
    public:
        RESTAPI_firmwaresHandler(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS}) {}

        void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;

        void DoGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
    };
}

#endif //UCENTRALFWS_RESTAPI_FIRMWARESHANDLER_H
