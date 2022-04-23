//
// Created by stephane bourque on 2021-07-19.
//

#pragma once

#include "framework/MicroService.h"

namespace OpenWifi {
    class RESTAPI_deviceReportHandler : public RESTAPIHandler {
    public:
        RESTAPI_deviceReportHandler(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L, RESTAPI_GenericServer & Server, uint64_t TransactionId, bool Internal)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS},
                                          Server,
                                          TransactionId,
                                 Internal) {}
        static auto PathName() { return std::list<std::string>{"/api/v1/deviceReport"};}
        void DoGet()  final;
        void DoDelete() final {};
        void DoPost() final {};
        void DoPut() final {};
    };
}
