//
// Created by stephane bourque on 2021-07-13.
//

#ifndef UCENTRALFMS_NEWCONNECTIONHANDLER_H
#define UCENTRALFMS_NEWCONNECTIONHANDLER_H


#include "SubSystemServer.h"
#include "uCentralTypes.h"

namespace uCentral {

    class NewConnectionHandler : public SubSystemServer, Poco::Runnable {
    public:

        static NewConnectionHandler *instance() {
            if (instance_ == nullptr) {
                instance_ = new NewConnectionHandler;
            }
            return instance_;
        }

        void run() override;
        int Start() override;
        void Stop() override;
        bool Update();

        void ConnectionReceived( const std::string & Key, const std::string & Message);

    private:
        static NewConnectionHandler      *instance_;
        Poco::Thread                Worker_;
        std::atomic_bool            Running_ = false;
        int                         WatcherId_=0;
        Types::StringPairQueue      NewConnections_;

        NewConnectionHandler() noexcept:
                SubSystemServer("ConnectionHandler", "NEWCONN-MGR", "connectionhandler") {
        }

    };
    inline NewConnectionHandler * NewConnectionHandler() { return NewConnectionHandler::instance(); };
}

#endif //UCENTRALFMS_NEWCONNECTIONHANDLER_H
