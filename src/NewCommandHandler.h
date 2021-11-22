//
// Created by stephane bourque on 2021-11-21.
//

#ifndef OWFMS_NEWCOMMANDHANDLER_H
#define OWFMS_NEWCOMMANDHANDLER_H

#include "framework/MicroService.h"
#include "framework/OpenWifiTypes.h"

namespace OpenWifi {

    class NewCommandHandler : public SubSystemServer, Poco::Runnable {
    public:
        static NewCommandHandler *instance() {
            static NewCommandHandler *instance_ = new NewCommandHandler;
            return instance_;
        }

        void run() override;
        int Start() override;
        void Stop() override;
        bool Update();
        void CommandReceived( const std::string & Key, const std::string & Message);

    private:
        Poco::Thread                Worker_;
        std::atomic_bool            Running_ = false;
        int                         WatcherId_=0;
        Types::StringPairQueue      NewCommands_;

        NewCommandHandler() noexcept:
            SubSystemServer("NewCommandHandler", "NEWCOM-MGR", "commanmdhandler") {
        }

    };
    inline NewCommandHandler * NewCommandHandler() { return NewCommandHandler::instance(); };

}

#endif //OWFMS_NEWCOMMANDHANDLER_H
