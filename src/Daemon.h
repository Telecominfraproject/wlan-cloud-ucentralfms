//
// Created by stephane bourque on 2021-05-07.
//

#ifndef UCENTRALFWS_DAEMON_H
#define UCENTRALFWS_DAEMON_H

#include "Poco/Util/Application.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/ErrorHandler.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/Crypto/RSAKey.h"

namespace uCentral {

    static const char * DAEMON_PROPERTIES_FILENAME = "ucentralfws.properties";
    static const char * DAEMON_ROOT_ENV_VAR = "UCENTRALFWS_ROOT";
    static const char * DAEMON_CONFIG_ENV_VAR = "UCENTRALFWS_CONFIG";
    static const char * DAEMON_APP_NAME = "uCentralFWS";

    class MyErrorHandler : public Poco::ErrorHandler {
    public:
        void exception(const Poco::Exception & E) override;
        void exception(const std::exception & E) override;
        void exception() override;
    private:

    };

    class Daemon : public Poco::Util::ServerApplication {
    public:
        int main(const ArgVec &args) override;
        void initialize(Application &self) override;
        void uninitialize() override;
        void reinitialize(Application &self) override;
        void defineOptions(Poco::Util::OptionSet &options) override;
        void handleHelp(const std::string &name, const std::string &value);
        void handleVersion(const std::string &name, const std::string &value);
        void handleDebug(const std::string &name, const std::string &value);
        void handleLogs(const std::string &name, const std::string &value);
        void handleConfig(const std::string &name, const std::string &value);
        void displayHelp();

        static std::string Version();
        const Poco::SharedPtr<Poco::Crypto::RSAKey> & Key() { return AppKey_; }
        std::string CreateUUID();
        static void Exit(int Reason);

        static Daemon *instance() {
            if (instance_ == nullptr) {
                instance_ = new Daemon;
            }
            return instance_;
        }


    private:
        static Daemon          *instance_;
        bool     HelpRequested_ = false;
        std::string             LogDir_;
        std::string             ConfigFileName_;
        Poco::UUIDGenerator     UUIDGenerator_;
        MyErrorHandler          AppErrorHandler_;
        Poco::SharedPtr<Poco::Crypto::RSAKey>	AppKey_ = nullptr;
        bool                    DebugMode_ = false;
    };

    namespace ServiceConfig {
        uint64_t GetInt(const std::string &Key,uint64_t Default);
        uint64_t GetInt(const std::string &Key);
        std::string GetString(const std::string &Key,const std::string & Default);
        std::string GetString(const std::string &Key);
        uint64_t GetBool(const std::string &Key,bool Default);
        uint64_t GetBool(const std::string &Key);
    }

    Daemon * instance();
}
#endif //UCENTRALFWS_DAEMON_H
