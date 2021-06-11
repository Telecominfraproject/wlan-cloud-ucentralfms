//
// Created by stephane bourque on 2021-06-09.
//

#include "KafkaNotifier.h"
#include <iostream>

#include "Daemon.h"

namespace uCentral::KafkaNotifier {
    Service *Service::instance_ = nullptr;

    int Start() {
        return Service::instance()->Start();
    }

    void Stop() {
        Service::instance()->Stop();
    }

    int RegisterTopicWatcher(const std::string &Topic, TopicNotifyFunction & F) {
        return Service::instance()->RegisterTopicWatcher(Topic, F);
    }

    void UnregisterTopicWatcher(const std::string &Topic, int FunctionId) {
        Service::instance()->UnregisterTopicWatcher(Topic, FunctionId);
    }

    Service::Service() noexcept:
        SubSystemServer("KafkaService", "KAFKA-MGR", "ucentral.kafka") {
    }

    void Service::run() {
        cppkafka::Configuration Config({
                                               { "group.id", 1 },
                                               // Disable auto commit
                                               { "enable.auto.commit", uCentral::ServiceConfig::GetBool("ucentral.kafka.auto.commit",false) },
                                               { "metadata.broker.list",uCentral::ServiceConfig::GetString("ucentral.kafka.brokerlist") }
                                       });

        cppkafka::Consumer Consumer(Config);

        Consumer.set_assignment_callback([this](const cppkafka::TopicPartitionList& partitions) {
            Logger_.information(Poco::format("Got assigned: %Lu...",(uint64_t )partitions.front().get_partition()));
        });
        Consumer.set_revocation_callback([this](const cppkafka::TopicPartitionList& partitions) {
            Logger_.information(Poco::format("Got revoked: %Lu...",(uint64_t )partitions.front().get_partition()));
        });

        std::vector<std::string>    Topics;
        for(const auto &i:Notifiers_)
            Topics.push_back(i.first);

        Consumer.subscribe(Topics);
        Running_ = true;
        while(Running_) {
            Poco::Thread::trySleep(3000);
            if(!Running_)
                break;
            cppkafka::Message Msg = Consumer.poll();
            if (Msg) {
                if (Msg.get_error()) {
                    if (!Msg.is_eof()) {
                        Logger_.error(Poco::format("Error: %s",Msg.get_error().to_string()));
                    }
                } else {
                    auto It = Notifiers_.find(Msg.get_topic());
                    if(It!=Notifiers_.end()) {
                        TopicNotifyFunctionList & FL = It->second;
                        for(auto &F:FL)
                            F.first(Msg.get_key(), Msg.get_payload());
                    }
                    Consumer.commit(Msg);
                }
            }
        }
    }

    int Service::RegisterTopicWatcher(const std::string &Topic, TopicNotifyFunction &F) {
        if(!Running_) {
            SubMutexGuard G(Mutex_);
            auto It = Notifiers_.find(Topic);
            if(It == Notifiers_.end()) {
                TopicNotifyFunctionList L;
                L.emplace(L.end(),std::make_pair(F,FunctionId_));
                Notifiers_[Topic] = std::move(L);
            } else {
                It->second.emplace(It->second.end(),std::make_pair(F,FunctionId_));
            }
            return FunctionId_++;
        } else {
            return 0;
        }
    }

    void Service::UnregisterTopicWatcher(const std::string &Topic, int Id) {
        if(!Running_) {
            SubMutexGuard G(Mutex_);
            auto It = Notifiers_.find(Topic);
            if(It != Notifiers_.end()) {
                TopicNotifyFunctionList & L = It->second;
                for(auto it=L.begin(); it!=L.end(); it++)
                    if(it->second == Id) {
                        L.erase(it);
                        break;
                    }
            }
        }
    }

    int Service::Start() {
        SubMutexGuard G(Mutex_);
        if(!Running_) {
            Worker_.start(*this);
        }
        return 0;
    }

    void Service::Stop() {
        {
            SubMutexGuard G(Mutex_);
            Running_ = false;
        }
        Worker_.wakeUp();
        Worker_.join();
    }
}