//
// Created by stephane bourque on 2021-07-19.
//

#ifndef UCENTRALFMS_RESTAPI_DEVICEREPORTHANDLER_H
#define UCENTRALFMS_RESTAPI_DEVICEREPORTHANDLER_H

#include "RESTAPI_handler.h"

namespace OpenWifi {
    class RESTAPI_deviceReportHandler : public RESTAPIHandler {
    public:
        RESTAPI_deviceReportHandler(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L, bool Internal)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS},
                                 Internal) {}
        static const std::list<const char *> PathName() { return std::list<const char *>{"/api/v1/deviceReport"};}
        void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;
        void DoGet(Poco::Net::HTTPServerRequest &Request, Poco::Net::HTTPServerResponse &Response);
    };
}


#endif //UCENTRALFMS_RESTAPI_DEVICEREPORTHANDLER_H
