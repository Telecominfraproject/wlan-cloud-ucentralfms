//
// Created by stephane bourque on 2021-05-09.
//

#ifndef UCENTRALFWS_RESTAPI_FIRMWARESHANDLER_H
#define UCENTRALFWS_RESTAPI_FIRMWARESHANDLER_H

#include "framework/MicroService.h"

namespace OpenWifi {
    class RESTAPI_firmwaresHandler : public RESTAPIHandler {
    public:
        RESTAPI_firmwaresHandler(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L, RESTAPI_GenericServer & Server, uint64_t TransactionId, bool Internal)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS},
                                          Server,
                                          TransactionId,
                                          Internal) {}

        static const std::list<const char *> PathName() { return std::list<const char *>{"/api/v1/firmwares"};}
        void DoGet()  final;
        void DoDelete() final {};
        void DoPost() final {};
        void DoPut() final {};
    };
}

#endif //UCENTRALFWS_RESTAPI_FIRMWARESHANDLER_H
