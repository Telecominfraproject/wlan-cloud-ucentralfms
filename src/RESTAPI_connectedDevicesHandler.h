//
// Created by stephane bourque on 2021-07-18.
//

#ifndef UCENTRALFMS_RESTAPI_CONNECTEDDEVICESHANDLER_H
#define UCENTRALFMS_RESTAPI_CONNECTEDDEVICESHANDLER_H


#include "RESTAPI_handler.h"

namespace uCentral {
    class RESTAPI_connectedDevicesHandler : public RESTAPIHandler {
    public:
        RESTAPI_connectedDevicesHandler(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS}) {}
        static const std::list<const char *> PathName() { return std::list<const char *>{"/api/v1/connectedDevices"};}
        void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;
        void DoGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
    };
}

#endif //UCENTRALFMS_RESTAPI_CONNECTEDDEVICESHANDLER_H
