//
// Created by stephane bourque on 2021-05-07.
//

#ifndef UCENTRALFWS_DAEMON_H
#define UCENTRALFWS_DAEMON_H

#include "framework/MicroService.h"
#include "framework/OpenWifiTypes.h"
#include "RESTObjects/RESTAPI_FMSObjects.h"
#include "Dashboard.h"

namespace OpenWifi {

    static const char * vDAEMON_PROPERTIES_FILENAME = "owfms.properties";
    static const char * vDAEMON_ROOT_ENV_VAR = "OWFMS_ROOT";
    static const char * vDAEMON_CONFIG_ENV_VAR = "OWFMS_CONFIG";
    static const char * vDAEMON_APP_NAME = uSERVICE_FIRMWARE.c_str();
    static const uint64_t vDAEMON_BUS_TIMER = 10000;

    class Daemon : public MicroService {
    public:
        explicit Daemon(const std::string & PropFile,
                        const std::string & RootEnv,
                        const std::string & ConfigEnv,
                        const std::string & AppName,
                        uint64_t 	BusTimer,
                        const SubSystemVec & SubSystems) :
                MicroService( PropFile, RootEnv, ConfigEnv, AppName, BusTimer, SubSystems) {};

        void PostInitialization(Poco::Util::Application &self);
        static Daemon *instance();
        inline void ResetDashboard() { DB_.Reset(); }
        inline void CreateDashboard() { DB_.Create(); }
        inline const FMSObjects::DeviceReport & GetDashboard() { return DB_.Report(); }

    private:
        static Daemon 				*instance_;
        DeviceDashboard             DB_;
    };

    inline Daemon * Daemon() { return Daemon::instance(); }
    inline void DaemonPostInitialization(Poco::Util::Application &self) {
        Daemon()->PostInitialization(self);
    }
}

#endif //UCENTRALFWS_DAEMON_H
