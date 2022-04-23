//
// Created by stephane bourque on 2021-07-18.
//

#ifndef UCENTRALFMS_RESTAPI_CONNECTEDDEVICESHANDLER_H
#define UCENTRALFMS_RESTAPI_CONNECTEDDEVICESHANDLER_H


#include "framework/MicroService.h"

namespace OpenWifi {
    class RESTAPI_connectedDevicesHandler : public RESTAPIHandler {
    public:
        RESTAPI_connectedDevicesHandler(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L, RESTAPI_GenericServer & Server, uint64_t TransactionId, bool Internal)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS},
                                          Server,
                                          TransactionId,
                                          Internal) {}
        static auto PathName() { return std::list<std::string>{"/api/v1/connectedDevices"};}
        void DoGet()  final;
        void DoDelete() final {};
        void DoPost() final {};
        void DoPut() final {};

    };
}

#endif //UCENTRALFMS_RESTAPI_CONNECTEDDEVICESHANDLER_H
