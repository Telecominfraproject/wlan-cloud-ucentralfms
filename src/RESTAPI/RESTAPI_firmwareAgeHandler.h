//
// Created by stephane bourque on 2021-07-16.
//

#ifndef UCENTRALFMS_RESTAPI_FIRMWAREAGEHANDLER_H
#define UCENTRALFMS_RESTAPI_FIRMWAREAGEHANDLER_H

#include "framework/MicroService.h"

namespace OpenWifi {
    class RESTAPI_firmwareAgeHandler : public RESTAPIHandler {
    public:
        RESTAPI_firmwareAgeHandler(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L, RESTAPI_GenericServer & Server, bool Internal)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS},
                                          Server,
                                          Internal) {}
        static const std::list<const char *> PathName() { return std::list<const char *>{"/api/v1/firmwareAge"};}
        void DoGet()  final;
        void DoDelete() final {};
        void DoPost() final {};
        void DoPut() final {};
    };
}

#endif //UCENTRALFMS_RESTAPI_FIRMWAREAGEHANDLER_H
