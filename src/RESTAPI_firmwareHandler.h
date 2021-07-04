//
// Created by stephane bourque on 2021-05-09.
//

#ifndef UCENTRALFWS_RESTAPI_FIRMWAREHANDLER_H
#define UCENTRALFWS_RESTAPI_FIRMWAREHANDLER_H

#include "RESTAPI_handler.h"

namespace uCentral {
    class RESTAPI_firmwareHandler : public RESTAPIHandler {
    public:
        RESTAPI_firmwareHandler(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_POST,
                                          Poco::Net::HTTPRequest::HTTP_PUT,
                                          Poco::Net::HTTPRequest::HTTP_DELETE,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS}) {}
        static const std::list<const char *> PathName() { return std::list<const char *>{"/api/v1/firmware/{uuid}"};}
        void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;
        void DoPost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
        void DoGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
        void DoDelete(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
        void DoPut(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
    };
}

#endif //UCENTRALFWS_RESTAPI_FIRMWAREHANDLER_H
