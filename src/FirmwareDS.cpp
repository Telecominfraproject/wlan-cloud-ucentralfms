//
// Created by stephane bourque on 2021-05-07.
//

#include <cstdlib>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "Poco/Util/Application.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Environment.h"
#include "Poco/Path.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/Socket.h"

#include "FirmwareDS.h"
#include "uStorageService.h"
#include "RESTAPI_server.h"
#include "uFileUploader.h"
#include "FWManager.h"

namespace uCentral {

    Daemon App;
    Daemon * instance() { return &App; }

    void MyErrorHandler::exception(const Poco::Exception & E) {
        Poco::Thread * CurrentThread = Poco::Thread::current();
        App.logger().log(E);
        App.logger().error(Poco::format("Exception occurred in %s",CurrentThread->getName()));
    }

    void MyErrorHandler::exception(const std::exception & E) {
        Poco::Thread * CurrentThread = Poco::Thread::current();
        App.logger().warning(Poco::format("std::exception on %s",CurrentThread->getName()));
    }

    void MyErrorHandler::exception() {
        Poco::Thread * CurrentThread = Poco::Thread::current();
        App.logger().warning(Poco::format("exception on %s",CurrentThread->getName()));
    }

    void Daemon::Exit(int Reason) {
        std::exit(Reason);
    }

    void Daemon::initialize(Application &self) {

        Poco::Net::initializeSSL();

        std::string Location = Poco::Environment::get("UCENTRALFWS_CONFIG",".");
        Poco::Path ConfigFile;

        ConfigFile = ConfigFileName_.empty() ? Location + "/ucentralfws.properties" : ConfigFileName_;

        if(!ConfigFile.isFile())
        {
            std::cerr << "uCentralFWS: Configuration " << ConfigFile.toString() << " does not seem to exist. Please set UCENTRALFWS_CONFIG env variable the path of the ucentralfws.properties file." << std::endl;
            std::exit(Poco::Util::Application::EXIT_CONFIG);
        }

        static const char * LogFilePathKey = "logging.channels.c2.path";

        loadConfiguration(ConfigFile.toString());

        if(LogDir_.empty()) {
            std::string OriginalLogFileValue = config().getString(LogFilePathKey);
            std::string RealLogFileValue = Poco::Path::expand(OriginalLogFileValue);
            config().setString(LogFilePathKey, RealLogFileValue);
        } else {
            config().setString(LogFilePathKey, LogDir_);
        }

        std::string KeyFile = Poco::Path::expand(config().getString("ucentralfws.service.key"));

        AppKey_ = Poco::SharedPtr<Poco::Crypto::RSAKey>(new Poco::Crypto::RSAKey("", KeyFile, ""));

        addSubsystem(uCentral::Storage::Service::instance());
        addSubsystem(uCentral::Auth::Service::instance());
        addSubsystem(uCentral::RESTAPI::Service::instance());
        addSubsystem(uCentral::uFileUploader::Service::instance());
        addSubsystem(uCentral::FWManager::Service::instance());

        ServerApplication::initialize(self);

        logger().information("Starting...");
    }

    void Daemon::uninitialize() {
        // add your own uninitialization code here
        ServerApplication::uninitialize();
    }

    void Daemon::reinitialize(Poco::Util::Application &self) {
        ServerApplication::reinitialize(self);
        // add your own reinitialization code here
    }

    void Daemon::defineOptions(Poco::Util::OptionSet &options) {
        ServerApplication::defineOptions(options);

        options.addOption(
                Poco::Util::Option("help", "", "display help information on command line arguments")
                        .required(false)
                        .repeatable(false)
                        .callback(Poco::Util::OptionCallback<Daemon>(this, &Daemon::handleHelp)));

        options.addOption(
                Poco::Util::Option("file", "", "specify the configuration file")
                        .required(false)
                        .repeatable(false)
                        .argument("file")
                        .callback(Poco::Util::OptionCallback<Daemon>(this, &Daemon::handleConfig)));

        options.addOption(
                Poco::Util::Option("debug", "", "to run in debug, set to true")
                        .required(false)
                        .repeatable(false)
                        .callback(Poco::Util::OptionCallback<Daemon>(this, &Daemon::handleDebug)));

        options.addOption(
                Poco::Util::Option("logs", "", "specify the log directory and file (i.e. dir/file.log)")
                        .required(false)
                        .repeatable(false)
                        .argument("dir")
                        .callback(Poco::Util::OptionCallback<Daemon>(this, &Daemon::handleLogs)));

        options.addOption(
                Poco::Util::Option("version", "", "get the version and quit.")
                        .required(false)
                        .repeatable(false)
                        .callback(Poco::Util::OptionCallback<Daemon>(this, &Daemon::handleVersion)));

    }

    std::string Daemon::Version() {
        std::string V = APP_VERSION;
        std::string B = BUILD_NUMBER;
        return V + "(" + B +  ")";
    }

    void Daemon::handleHelp(const std::string &name, const std::string &value) {
        HelpRequested_ = true;
        displayHelp();
        stopOptionsProcessing();
    }

    void Daemon::handleVersion(const std::string &name, const std::string &value) {
        HelpRequested_ = true;
        std::cout << Version() << std::endl;
        stopOptionsProcessing();
    }

    void Daemon::handleDebug(const std::string &name, const std::string &value) {
        if(value == "true")
            DebugMode_ = true ;
    }

    void Daemon::handleLogs(const std::string &name, const std::string &value) {
        LogDir_ = value;
    }

    void Daemon::handleConfig(const std::string &name, const std::string &value) {
        ConfigFileName_ = value;
    }

    void Daemon::displayHelp() {
        Poco::Util::HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A uCentral gateway implementation for TIP.");
        helpFormatter.format(std::cout);
    }

    std::string Daemon::CreateUUID() {
        return UUIDGenerator_.create().toString();
    }

    int Daemon::main(const ArgVec &args) {

        Poco::ErrorHandler::set(&AppErrorHandler_);

        if (!HelpRequested_) {
            Poco::Logger &logger = Poco::Logger::get("uCentralFWS");
            logger.notice(Poco::format("Starting uCentralFWS version %s.",Version()));

            if(Poco::Net::Socket::supportsIPv6()) {
                logger.information("System supports IPv6.");
            }
            else
                logger.information("System does NOT supported IPv6.");

            uCentral::Storage::Start();
            uCentral::Auth::Start();
            uCentral::RESTAPI::Start();
            uCentral::uFileUploader::Start();
            uCentral::FWManager::Start();

            Poco::Thread::sleep(2000);

//            uCentral::Auth::APIKeyEntry E = uCentral::Auth::Service::instance()->GetFirst();
//            uCentral::FWManager::AddJob("job1", E);

            App.waitForTerminationRequest();

            uCentral::FWManager::Stop();
            uCentral::uFileUploader::Stop();
            uCentral::RESTAPI::Stop();
            uCentral::Auth::Stop();
            uCentral::Storage::Stop();

            logger.notice("Stopped uCentralFWS...");
        }

        return Application::EXIT_OK;
    }

    namespace ServiceConfig {

        uint64_t GetInt(const std::string &Key,uint64_t Default) {
            return (uint64_t) App.config().getInt64(Key,Default);
        }

        uint64_t GetInt(const std::string &Key) {
            return App.config().getInt(Key);
        }

        uint64_t GetBool(const std::string &Key,bool Default) {
            return App.config().getBool(Key,Default);
        }

        uint64_t GetBool(const std::string &Key) {
            return App.config().getBool(Key);
        }

        std::string GetString(const std::string &Key,const std::string & Default) {
            std::string R = App.config().getString(Key, Default);
            return Poco::Path::expand(R);
        }

        std::string GetString(const std::string &Key) {
            std::string R = App.config().getString(Key);
            return Poco::Path::expand(R);
        }
    }
}

int main(int argc, char **argv) {
    try {
        return uCentral::App.run(argc, argv);
    } catch (Poco::Exception &exc) {
        std::cerr << exc.displayText() << std::endl;
        return Poco::Util::Application::EXIT_SOFTWARE;
    }
}
