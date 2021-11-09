//
// Created by stephane bourque on 2021-07-13.
//

#ifndef UCENTRALFMS_NEWCONNECTIONHANDLER_H
#define UCENTRALFMS_NEWCONNECTIONHANDLER_H


#include "framework/MicroService.h"
#include "framework/OpenWifiTypes.h"

namespace OpenWifi {

    class NewConnectionHandler : public SubSystemServer, Poco::Runnable {
    public:

        static NewConnectionHandler *instance() {
            static NewConnectionHandler instance;
            return &instance;
        }

        void run() override;
        int Start() override;
        void Stop() override;
        bool Update();

        void ConnectionReceived( const std::string & Key, const std::string & Message);

    private:
        Poco::Thread                Worker_;
        std::atomic_bool            Running_ = false;
        int                         ConnectionWatcherId_=0;
        int                         HealthcheckWatcherId_=0;
        Types::StringPairQueue      NewConnections_;

        NewConnectionHandler() noexcept:
                SubSystemServer("ConnectionHandler", "NEWCONN-MGR", "connectionhandler") {
        }

    };
    inline NewConnectionHandler * NewConnectionHandler() { return NewConnectionHandler::instance(); };
}

#endif //UCENTRALFMS_NEWCONNECTIONHANDLER_H
