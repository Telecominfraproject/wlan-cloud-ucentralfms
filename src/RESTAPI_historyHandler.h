//
// Created by stephane bourque on 2021-07-13.
//

#ifndef UCENTRALFMS_RESTAPI_HISTORYHANDLER_H
#define UCENTRALFMS_RESTAPI_HISTORYHANDLER_H


#include "RESTAPI_handler.h"

namespace uCentral {
    class RESTAPI_historyHandler : public RESTAPIHandler {
    public:
        RESTAPI_historyHandler(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L, bool Internal)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_DELETE,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS},
                                          Internal) {}
        static const std::list<const char *> PathName() { return std::list<const char *>{"/api/v1/revisionHistory/{serialNumber}"};}
        void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;
        void DoGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
        void DoDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
    };
}


#endif //UCENTRALFMS_RESTAPI_HISTORYHANDLER_H
