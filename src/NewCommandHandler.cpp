//
// Created by stephane bourque on 2021-11-21.
//

#include "NewCommandHandler.h"
#include "StorageService.h"

namespace OpenWifi {

    void NewCommandHandler::run() {
        Running_ = true ;
        while(Running_) {
            Poco::Thread::trySleep(2000);

            if(!Running_)
                break;

            while(!NewCommands_.empty()) {
                if(!Running_)
                    break;

                Types::StringPair  S;
                {
                    std::lock_guard G(Mutex_);
                    S = NewCommands_.front();
                    NewCommands_.pop();
                }

                try {
                    auto SerialNumber = S.first;
                    auto M = nlohmann::json::parse(S.second);

                    std::string EndPoint;

                    if(M.contains(uCentralProtocol::SYSTEM)) {
                        auto SystemObj = M[uCentralProtocol::SYSTEM];
                        if(SystemObj.contains(uCentralProtocol::HOST))
                            EndPoint = SystemObj[uCentralProtocol::HOST];
                    }

                    if(M.contains(uCentralProtocol::PAYLOAD)) {
                        auto PayloadSection = M[uCentralProtocol::PAYLOAD];
                        if(PayloadSection.contains("command")) {
                            auto Command = PayloadSection["command"];
                            if(Command=="delete_device") {
                                auto pSerialNumber = PayloadSection["payload"]["serialNumber"];
                                if(pSerialNumber==SerialNumber) {
                                    Logger().debug(Poco::format("Removing device '%s' from upgrade history.",SerialNumber));
                                    StorageService()->DeleteHistory(SerialNumber);
                                    Logger().debug(Poco::format("Removing device '%s' from device table.",SerialNumber));
                                    StorageService()->DeleteDevice(SerialNumber);
                                }
                            }
                        }
                    }
                } catch (const Poco::Exception &E) {
                    Logger().log(E);
                }
            }
        }
    };

    int NewCommandHandler::Start() {
        Types::TopicNotifyFunction F = [this](std::string s1,std::string s2) { this->CommandReceived(s1,s2); };
        WatcherId_ = KafkaManager()->RegisterTopicWatcher(KafkaTopics::COMMAND, F);
        Worker_.start(*this);
        return 0;
    };

    void NewCommandHandler::Stop() {
        KafkaManager()->UnregisterTopicWatcher(KafkaTopics::COMMAND, WatcherId_);
        Running_ = false;
        Worker_.wakeUp();
        Worker_.join();
    };

    bool NewCommandHandler::Update() {
        Worker_.wakeUp();
        return true;
    }

    void NewCommandHandler::CommandReceived( const std::string & Key, const std::string & Message) {
        std::lock_guard G(Mutex_);
        NewCommands_.push(std::make_pair(Key,Message));
    }
}