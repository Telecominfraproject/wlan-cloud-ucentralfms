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

    static const char * vDAEMON_PROPERTIES_FILENAME = "ucentralfws.properties";
    static const char * vDAEMON_ROOT_ENV_VAR = "UCENTRALFWS_ROOT";
    static const char * vDAEMON_CONFIG_ENV_VAR = "UCENTRALFWS_CONFIG";
    static const char * vDAEMON_APP_NAME = "uCentralFWS";

    class Daemon : public MicroService {
    public:
        explicit Daemon(std::string PropFile,
                        std::string RootEnv,
                        std::string ConfigEnv,
                        std::string AppName,
                        Types::SubSystemVec SubSystems) :
                MicroService( PropFile, RootEnv, ConfigEnv, AppName, SubSystems) {};

        bool AutoProvisioning() const { return AutoProvisioning_ ; }
        [[nodiscard]] std::string IdentifyDevice(const std::string & Compatible) const;
        void initialize(Poco::Util::Application &self);
        static Daemon *instance();
    private:
        static Daemon 				*instance_;
        bool                        AutoProvisioning_ = false;
        Types::StringMapStringSet   DeviceTypeIdentifications_;
    };

    inline Daemon * Daemon() { return Daemon::instance(); }
}

#endif //UCENTRALFWS_DAEMON_H
