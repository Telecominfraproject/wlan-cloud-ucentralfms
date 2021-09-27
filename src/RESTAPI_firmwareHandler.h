//
// Created by stephane bourque on 2021-05-09.
//

#ifndef UCENTRALFWS_RESTAPI_FIRMWAREHANDLER_H
#define UCENTRALFWS_RESTAPI_FIRMWAREHANDLER_H

#include "RESTAPI_handler.h"

namespace OpenWifi {
    class RESTAPI_firmwareHandler : public RESTAPIHandler {
    public:
        RESTAPI_firmwareHandler(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L, RESTAPI_GenericServer & Server, bool Internal)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_POST,
                                          Poco::Net::HTTPRequest::HTTP_PUT,
                                          Poco::Net::HTTPRequest::HTTP_DELETE,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS},
                                          Server,
                                          Internal) {}
        static const std::list<const char *> PathName() { return std::list<const char *>{"/api/v1/firmware/{id}"};}
        void DoGet()  final;
        void DoDelete() final;
        void DoPost() final;
        void DoPut() final;
    };
}

#endif //UCENTRALFWS_RESTAPI_FIRMWAREHANDLER_H
