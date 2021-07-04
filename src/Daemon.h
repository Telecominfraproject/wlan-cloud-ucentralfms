//
// Created by stephane bourque on 2021-05-07.
//

#include <list>

#ifndef UCENTRALFWS_DAEMON_H
#define UCENTRALFWS_DAEMON_H

#include "Poco/Util/Application.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/ErrorHandler.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/Crypto/RSAKey.h"
#include "Poco/Crypto/CipherFactory.h"
#include "Poco/Crypto/Cipher.h"

#include "MicroService.h"
#include "uCentralTypes.h"

namespace uCentral {

    static const char * vDAEMON_PROPERTIES_FILENAME = "ucentralfms.properties";
    static const char * vDAEMON_ROOT_ENV_VAR = "UCENTRALFMS_ROOT";
    static const char * vDAEMON_CONFIG_ENV_VAR = "UCENTRALFMS_CONFIG";
    static const char * vDAEMON_APP_NAME = "uCentralFMS";
    static const uint64_t vDAEMON_BUS_TIMER = 10000;

    class Daemon : public MicroService {
    public:
        explicit Daemon(std::string PropFile,
                        std::string RootEnv,
                        std::string ConfigEnv,
                        std::string AppName,
                        uint64_t 	BusTimer,
                        Types::SubSystemVec SubSystems) :
                MicroService( PropFile, RootEnv, ConfigEnv, AppName, BusTimer, SubSystems) {};

        void initialize(Poco::Util::Application &self);
        static Daemon *instance();
    private:
        static Daemon 				*instance_;
    };

    inline Daemon * Daemon() { return Daemon::instance(); }
}

#endif //UCENTRALFWS_DAEMON_H
