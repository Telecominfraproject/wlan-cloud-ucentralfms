//
// Created by stephane bourque on 2021-06-09.
//

#ifndef UCENTRALFWS_KAFKANOTIFIER_H
#define UCENTRALFWS_KAFKANOTIFIER_H

#include "SubSystemServer.h"
#include "cppkafka/cppkafka.h"

namespace uCentral::KafkaNotifier {
    typedef std::function<void(std::string,std::string)>    TopicNotifyFunction;
    typedef std::list<std::pair<TopicNotifyFunction,int>>   TopicNotifyFunctionList;
    typedef std::map<std::string, TopicNotifyFunctionList>  NotifyTable;

    int Start();
    void Stop();
    int RegisterTopicWatcher(const std::string &Topic, TopicNotifyFunction & F);
    void UnregisterTopicWatcher(const std::string &Topic, int FunctionId);

    class Service: public SubSystemServer, Poco::Runnable {
        public:
            Service() noexcept;
            friend int Start();
            friend void Stop();
            friend int RegisterTopicWatcher(const std::string &Topic, TopicNotifyFunction & F);
            friend void UnregisterTopicWatcher(const std::string &Topic, int FunctionId);

            static Service *instance() {
                if (instance_ == nullptr) {
                    instance_ = new Service;
                }
                return instance_;
            }

            void run() override;

        private:
            static Service           *instance_;
            Poco::Thread              Worker_;
            std::atomic_bool          Running_ = false;
            NotifyTable               Notifiers_;
            int                       FunctionId_=1;
            std::unique_ptr<cppkafka::Configuration>    Config_;
            int Start() override;
            void Stop() override;

            int  RegisterTopicWatcher(const std::string &Topic, TopicNotifyFunction & F);
            void UnregisterTopicWatcher(const std::string &Topic, int FunctionId);
    };
}


#endif //UCENTRALFWS_KAFKANOTIFIER_H
