//
// Created by stephane bourque on 2021-05-09.
//

#ifndef UCENTRALFWS_RESTAPI_FIRMWARESHANDLER_H
#define UCENTRALFWS_RESTAPI_FIRMWARESHANDLER_H

#include "RESTAPI_handler.h"

namespace uCentral {
    class RESTAPI_firmwaresHandler : public RESTAPIHandler {
    public:
        RESTAPI_firmwaresHandler(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L, bool Internal)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS},
                                          Internal) {}

        void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;
        static const std::list<const char *> PathName() { return std::list<const char *>{"/api/v1/firmwares"};}
        void DoGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
    };
}

#endif //UCENTRALFWS_RESTAPI_FIRMWARESHANDLER_H
