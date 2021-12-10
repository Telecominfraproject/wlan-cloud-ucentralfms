//
// Created by stephane bourque on 2021-10-23.
//

#include "framework/MicroService.h"

#include "RESTAPI/RESTAPI_firmwareHandler.h"
#include "RESTAPI/RESTAPI_firmwaresHandler.h"
#include "RESTAPI/RESTAPI_firmwareAgeHandler.h"
#include "RESTAPI/RESTAPI_connectedDeviceHandler.h"
#include "RESTAPI/RESTAPI_connectedDevicesHandler.h"
#include "RESTAPI/RESTAPI_historyHandler.h"
#include "RESTAPI/RESTAPI_deviceReportHandler.h"

namespace OpenWifi {

    Poco::Net::HTTPRequestHandler * RESTAPI_ExtRouter(const char *Path, RESTAPIHandler::BindingMap &Bindings,
                                                            Poco::Logger & L, RESTAPI_GenericServer & S, uint64_t TransactionId) {
        return  RESTAPI_Router<
            RESTAPI_firmwaresHandler,
            RESTAPI_firmwareHandler,
            RESTAPI_system_command,
            RESTAPI_firmwareAgeHandler,
            RESTAPI_connectedDevicesHandler,
            RESTAPI_connectedDeviceHandler,
            RESTAPI_historyHandler,
            RESTAPI_deviceReportHandler
            >(Path,Bindings,L, S, TransactionId);
    }

    Poco::Net::HTTPRequestHandler * RESTAPI_IntRouter(const char *Path, RESTAPIHandler::BindingMap &Bindings,
                                                            Poco::Logger & L, RESTAPI_GenericServer & S, uint64_t TransactionId) {
        return RESTAPI_Router_I<
            RESTAPI_firmwaresHandler,
            RESTAPI_firmwareHandler,
            RESTAPI_system_command,
            RESTAPI_connectedDevicesHandler,
            RESTAPI_connectedDeviceHandler
            >(Path, Bindings, L, S, TransactionId);
    }
}