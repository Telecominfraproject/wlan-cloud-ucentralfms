//
// Created by stephane bourque on 2021-05-09.
//

#include "Poco/URI.h"

#include "RESTAPI_server.h"
#include "Utils.h"
#include "RESTAPI_handler.h"

#include "RESTAPI_firmwareHandler.h"
#include "RESTAPI_firmwaresHandler.h"
#include "RESTAPI_system_command.h"
#include "RESTAPI_firmwareAgeHandler.h"
#include "RESTAPI_connectedDeviceHandler.h"
#include "RESTAPI_connectedDevicesHandler.h"
#include "RESTAPI_historyHandler.h"
#include "RESTAPI_deviceReportHandler.h"

namespace OpenWifi {

    class RESTAPI_server *RESTAPI_server::instance_ = nullptr;

    RESTAPI_server::RESTAPI_server() noexcept:
            SubSystemServer("RESTAPIServer", "RESTAPIServer", "openwifi.restapi")
    {
    }

    int RESTAPI_server::Start() {
        Logger_.information("Starting.");
        Server_.InitLogging();

        for(const auto & Svr: ConfigServersList_) {
            Logger_.information(Poco::format("Starting: %s:%s Keyfile:%s CertFile: %s", Svr.Address(), std::to_string(Svr.Port()),
                                             Svr.KeyFile(),Svr.CertFile()));

            auto Sock{Svr.CreateSecureSocket(Logger_)};

            Svr.LogCert(Logger_);
            if(!Svr.RootCA().empty())
                Svr.LogCas(Logger_);

            auto Params = new Poco::Net::HTTPServerParams;
            Params->setMaxThreads(50);
            Params->setMaxQueued(200);
            Params->setKeepAlive(true);

            auto NewServer = std::make_unique<Poco::Net::HTTPServer>(new RequestHandlerFactory(Server_), Pool_, Sock, Params);
            NewServer->start();
            RESTServers_.push_back(std::move(NewServer));
        }

        return 0;
    }

    Poco::Net::HTTPRequestHandler *RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest & Request) {
        Poco::URI uri(Request.getURI());
        auto *Path = uri.getPath().c_str();
        RESTAPIHandler::BindingMap Bindings;

        // std::cout << "Path: " << Request.getURI() << std::endl;

        return  RESTAPI_Router<
                RESTAPI_firmwaresHandler,
                RESTAPI_firmwareHandler,
                RESTAPI_system_command,
                RESTAPI_firmwareAgeHandler,
                RESTAPI_connectedDevicesHandler,
                RESTAPI_connectedDeviceHandler,
                RESTAPI_historyHandler,
                RESTAPI_deviceReportHandler
                >(Path,Bindings,Logger_, Server_);
    }

    void RESTAPI_server::Stop() {
        Logger_.information("Stopping ");
        for( const auto & svr : RESTServers_ )
            svr->stop();
    }

}  // namespace
