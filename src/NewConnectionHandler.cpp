//
// Created by stephane bourque on 2021-07-13.
//

#include "NewConnectionHandler.h"
#include "Kafka_topics.h"
#include "KafkaManager.h"
#include "uCentralTypes.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"

namespace uCentral {
    class NewConnectionHandler *NewConnectionHandler::instance_ = nullptr;

    void NewConnectionHandler::run() {
        Running_ = true ;
        while(Running_) {
            Poco::Thread::trySleep(2000);

            if(!Running_)
                break;

            while(!NewConnections_.empty()) {
                if(!Running_)
                    break;

                std::cout << "New connection..." << std::endl;
                Types::StringPair  S;
                {
                    SubMutexGuard G(Mutex_);
                    S = NewConnections_.front();
                    NewConnections_.pop();
                }
                auto SerialNumber = S.first;

                Poco::JSON::Parser  Parser;
                auto Object = Parser.parse(S.second).extract<Poco::JSON::Object::Ptr>();

                if(Object->has("capabilities") && Object->isObject("capabilities")) {
                    std::cout << "New connection..." << std::endl;
                    auto Capabilities = Object->getObject("capabilities");
                    std::cout << "Compatible: " << Capabilities->get("compatible").toString() << std::endl;
                }
            }
        }
    };

    int NewConnectionHandler::Start() {
        Types::TopicNotifyFunction F = [this](std::string s1,std::string s2) { this->ConnectionReceived(s1,s2); };
        WatcherId_ = KafkaManager()->RegisterTopicWatcher(KafkaTopics::CONNECTION, F);
        Worker_.start(*this);
        return 0;
    };

    void NewConnectionHandler::Stop() {
        KafkaManager()->UnregisterTopicWatcher(KafkaTopics::CONNECTION, WatcherId_);
        Running_ = false;
        Worker_.wakeUp();
        Worker_.join();
    };

    bool NewConnectionHandler::Update() {
        Worker_.wakeUp();
        return true;
    }

    void NewConnectionHandler::ConnectionReceived( const std::string & Key, const std::string & Message) {
        SubMutexGuard G(Mutex_);
        NewConnections_.push(std::make_pair(Key,Message));
        std::cout << "New connection..." << std::endl;
    }
}