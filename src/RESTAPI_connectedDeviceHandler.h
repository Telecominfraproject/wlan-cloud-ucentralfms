//
// Created by stephane bourque on 2021-07-18.
//

#ifndef UCENTRALFMS_RESTAPI_CONNECTEDDEVICEHANDLER_H
#define UCENTRALFMS_RESTAPI_CONNECTEDDEVICEHANDLER_H

#include "RESTAPI_handler.h"

namespace uCentral {
    class RESTAPI_connectedDeviceHandler : public RESTAPIHandler {
    public:
        RESTAPI_connectedDeviceHandler(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS}) {}
        static const std::list<const char *> PathName() { return std::list<const char *>{"/api/v1/connectedDevice/{serialNumber}"};}
        void handleRequest(Poco::Net::HTTPServerRequest &Request, Poco::Net::HTTPServerResponse &Response) override;
        void DoGet(Poco::Net::HTTPServerRequest &Request, Poco::Net::HTTPServerResponse &Response);
    };
}

#endif //UCENTRALFMS_RESTAPI_CONNECTEDDEVICEHANDLER_H
