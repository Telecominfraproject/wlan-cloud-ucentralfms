//
// Created by stephane bourque on 2021-10-23.
//

#include "framework/RESTAPI_Handler.h"
#include "RESTAPI/RESTAPI_firmwareHandler.h"
#include "RESTAPI/RESTAPI_firmwaresHandler.h"
#include "RESTAPI/RESTAPI_firmwareAgeHandler.h"
#include "RESTAPI/RESTAPI_connectedDeviceHandler.h"
#include "RESTAPI/RESTAPI_connectedDevicesHandler.h"
#include "RESTAPI/RESTAPI_historyHandler.h"
#include "RESTAPI/RESTAPI_deviceReportHandler.h"
#include "RESTAPI/RESTAPI_deviceInformation_handler.h"
#include "framework/RESTAPI_SystemCommand.h"
#include "framework/RESTAPI_WebSocketServer.h"

namespace OpenWifi {

    Poco::Net::HTTPRequestHandler * RESTAPI_ExtRouter(const std::string &Path, RESTAPIHandler::BindingMap &Bindings,
                                                            Poco::Logger & L, RESTAPI_GenericServerAccounting & S, uint64_t TransactionId) {
        return  RESTAPI_Router<
                RESTAPI_firmwaresHandler,
                RESTAPI_firmwareHandler,
                RESTAPI_system_command,
                RESTAPI_firmwareAgeHandler,
                RESTAPI_connectedDevicesHandler,
                RESTAPI_connectedDeviceHandler,
                RESTAPI_historyHandler,
                RESTAPI_deviceReportHandler,
                RESTAPI_deviceInformation_handler,
                RESTAPI_webSocketServer
            >(Path,Bindings,L, S, TransactionId);
    }

    Poco::Net::HTTPRequestHandler * RESTAPI_IntRouter(const std::string &Path, RESTAPIHandler::BindingMap &Bindings,
                                                            Poco::Logger & L, RESTAPI_GenericServerAccounting & S, uint64_t TransactionId) {
        return RESTAPI_Router_I<
                RESTAPI_firmwaresHandler,
                RESTAPI_firmwareHandler,
                RESTAPI_system_command,
                RESTAPI_firmwareAgeHandler,
                RESTAPI_connectedDevicesHandler,
                RESTAPI_connectedDeviceHandler,
                RESTAPI_historyHandler,
                RESTAPI_deviceReportHandler,
                RESTAPI_deviceInformation_handler
            >(Path, Bindings, L, S, TransactionId);
    }
}