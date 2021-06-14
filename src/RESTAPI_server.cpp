//
// Created by stephane bourque on 2021-05-09.
//

#include "Poco/URI.h"

#include "RESTAPI_server.h"
#include "Utils.h"
#include "RESTAPI_handler.h"

#include "RESTAPI_unknownRequestHandler.h"
#include "RESTAPI_oauth2Handler.h"
#include "RESTAPI_callbackHandler.h"
#include "RESTAPI_callbacksHandler.h"
#include "RESTAPI_firmwareHandler.h"
#include "RESTAPI_firmwaresHandler.h"
#include "RESTAPI_latestFirmwareListHandler.h"
#include "RESTAPI_callbackChannel.h"
#include "RESTAPI_newFirmwareAvailable.h"
#include "RESTAPI_system_command.h"

namespace uCentral {

    class RESTAPI_server *RESTAPI_server::instance_ = nullptr;

    RESTAPI_server::RESTAPI_server() noexcept:
            SubSystemServer("RESTAPIServer", "RESTAPIServer", "ucentralfws.restapi")
    {
    }

    int RESTAPI_server::Start() {
        Logger_.information("Starting.");

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
            uint64_t T = 45000;
            Params->setKeepAliveTimeout(T);
            Params->setMaxKeepAliveRequests(200);
            Params->setTimeout(T + 10000);

            auto NewServer = std::make_unique<Poco::Net::HTTPServer>(new RequestHandlerFactory, Pool_, Sock, Params);
            NewServer->start();
            RESTServers_.push_back(std::move(NewServer));
        }

        return 0;
    }

    Poco::Net::HTTPRequestHandler *RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest & Request) {

        Logger_.debug(Poco::format("REQUEST(%s): %s %s", uCentral::Utils::FormatIPv6(Request.clientAddress().toString()), Request.getMethod(), Request.getURI()));

        Poco::URI uri(Request.getURI());
        auto *Path = uri.getPath().c_str();
        RESTAPIHandler::BindingMap bindings;

        if (RESTAPIHandler::ParseBindings(Path, "/api/v1/oauth2", bindings)) {
            return new RESTAPI_oauth2Handler(bindings, Logger_);
        } else if (RESTAPIHandler::ParseBindings(Path, "/api/v1/oauth2/{token}", bindings)) {
            return new RESTAPI_oauth2Handler(bindings, Logger_);
        } else if (RESTAPIHandler::ParseBindings(Path, "/api/v1/firmwares", bindings)) {
            return new RESTAPI_firmwaresHandler(bindings, Logger_);
        } else if (RESTAPIHandler::ParseBindings(Path, "/api/v1/firmware/{uuid}", bindings)) {
            return new RESTAPI_firmwareHandler(bindings, Logger_);
        } else if (RESTAPIHandler::ParseBindings(Path, "/api/v1/callbacks", bindings)) {
            return new RESTAPI_callbacksHandler(bindings, Logger_);
        }  else if (RESTAPIHandler::ParseBindings(Path, "/api/v1/callback/{uuid}", bindings)) {
            return new RESTAPI_callbackHandler(bindings, Logger_);
        } else if (RESTAPIHandler::ParseBindings(Path, "/api/v1/latestFirmwareList", bindings)) {
            return new RESTAPI_latestFirmwareListHandler(bindings, Logger_);
        } else if (RESTAPIHandler::ParseBindings(Path, "/api/v1/callbackChannel", bindings)) {
            return new RESTAPI_callbackChannel(bindings, Logger_);
        } else if (RESTAPIHandler::ParseBindings(Path, "/api/v1/newFirmwareAvailable", bindings)) {
            return new RESTAPI_newFirmwareAvailable(bindings, Logger_);
        } else if (RESTAPIHandler::ParseBindings(Path, "/api/v1/system", bindings)) {
            return new RESTAPI_system_command(bindings, Logger_);
        }

        return new RESTAPI_UnknownRequestHandler(bindings,Logger_);
    }

    void RESTAPI_server::Stop() {
        Logger_.information("Stopping ");
        for( const auto & svr : RESTServers_ )
            svr->stop();
    }

}  // namespace