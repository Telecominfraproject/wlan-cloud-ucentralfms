//
// Created by stephane bourque on 2021-05-07.
//

#ifndef UCENTRALFWS_UFIRMWAREDS_H
#define UCENTRALFWS_UFIRMWAREDS_H

#include "Poco/Util/Application.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/ErrorHandler.h"
#include "Poco/UUIDGenerator.h"

namespace uCentral {

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
        void Exit(int Reason);

    private:
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
#endif //UCENTRALFWS_UFIRMWAREDS_H
