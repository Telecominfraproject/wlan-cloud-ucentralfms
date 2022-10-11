//
// Created by stephane bourque on 2021-07-13.
//

#pragma once

#include "framework/SubSystemServer.h"
#include "framework/OpenWifiTypes.h"

namespace OpenWifi {

    class NewConnectionHandler : public SubSystemServer, Poco::Runnable {
    public:

        static auto instance() {
            static auto instance_ = new NewConnectionHandler;
            return instance_;
        }

        void run() override;
        int Start() override;
        void Stop() override;
        bool Update();

        void ConnectionReceived( const std::string & Key, const std::string & Message);

    private:
        Poco::Thread                Worker_;
        std::atomic_bool            Running_ = false;
        uint64_t                    ConnectionWatcherId_=0;
        Types::StringPairQueue      NewConnections_;

        NewConnectionHandler() noexcept:
                SubSystemServer("ConnectionHandler", "NEWCONN-MGR", "connectionhandler") {
        }

    };
    inline auto NewConnectionHandler() { return NewConnectionHandler::instance(); };
}

