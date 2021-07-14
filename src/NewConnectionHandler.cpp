//
// Created by stephane bourque on 2021-07-13.
//

#include "NewConnectionHandler.h"
#include "Kafka_topics.h"
#include "KafkaManager.h"
#include "uCentralTypes.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "StorageService.h"
#include "LatestFirmwareCache.h"

/*

{ "system" : { "id" : 6715803232063 , "host" : "https://localhost:17002" } ,
 "payload" : "{"capabilities":{"compatible":"linksys_ea8300","model":"Linksys EA8300 (Dallas)","network":{"lan":["eth0"],"wan":["eth1"]},"platform":"ap","switch":{"switch0":{"enable":true,"ports":[{"device":"eth0","need_tag":false,"num":0,"want_untag":true},{"num":1,"role":"lan"},{"num":2,"role":"lan"},{"num":3,"role":"lan"},{"num":4,"role":"lan"}],"reset":true,"roles":[{"device":"eth0","ports":"1 2 3 4 0","role":"lan"}]}},"wifi":{"platform/soc/a000000.wifi":{"band":["2G"],"channels":[1,2,3,4,5,6,7,8,9,10,11],"frequencies":[2412,2417,2422,2427,2432,2437,2442,2447,2452,2457,2462],"ht_capa":6639,"htmode":["HT20","HT40","VHT20","VHT40","VHT80"],"rx_ant":3,"tx_ant":3,"vht_capa":865687986},"platform/soc/a800000.wifi":{"band":["5G"],"channels":[36,40,44,48,52,56,60,64],"frequencies":[5180,5200,5220,5240,5260,5280,5300,5320],"ht_capa":6639,"htmode":["HT20","HT40","VHT20","VHT40","VHT80"],"rx_ant":3,"tx_ant":3,"vht_capa":865687986},"soc/40000000.pci/pci0000:00/0000:00:00.0/0000:01:00.0":{"band":["5G"],"channels":[100,104,108,112,116,120,124,128,132,136,140,144,149,153,157,161,165],"frequencies":[5500,5520,5540,5560,5580,5600,5620,5640,5660,5680,5700,5720,5745,5765,5785,5805,5825],"ht_capa":6639,"htmode":["HT20","HT40","VHT20","VHT40","VHT80"],"rx_ant":3,"tx_ant":3,"vht_capa":865696178}}},"firmware":"OpenWrt 21.02-SNAPSHOT r16011+53-6fd65c6573 / TIP-devel-0825cb93","serial":"24f5a207a130","uuid":1623866223}}



 */

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

                Types::StringPair  S;
                {
                    SubMutexGuard G(Mutex_);
                    S = NewConnections_.front();
                    NewConnections_.pop();
                }
                auto SerialNumber = S.first;
                Poco::JSON::Parser  Parser;
                auto Object = Parser.parse(S.second).extract<Poco::JSON::Object::Ptr>();

                std::string compatible, serialNumber, firmware;
                if(Object->has("payload")) {
                    auto PayloadObj = Object->getObject("payload");
                    if(PayloadObj->has("capabilities")) {
                        auto CapObj = PayloadObj->getObject("capabilities");
                        if(CapObj->has("compatible")) {
                            compatible = CapObj->get("compatible").toString();
                            serialNumber = PayloadObj->get("serial").toString();
                            firmware = PayloadObj->get("firmware").toString();
                            std::cout << "Compatible: " << CapObj->get("compatible").toString() << std::endl;
                            FMSObjects::Firmware    CurrentFirmware;
                            FMSObjects::Firmware    LatestFirmware;
                            if(Storage()->GetFirmwareByRevision(firmware,compatible,CurrentFirmware)) {
                                LatestFirmwareCacheEntry    LE;
                                if(LatestFirmwareCache()->FindLatestFirmware(compatible,LE)) {
                                    Storage()->GetFirmware(LE.Id,LatestFirmware);
                                    std::cout << "Firmware is " << LatestFirmware.imageDate - CurrentFirmware.imageDate << " seconds out of date" << std::endl;
                                }
                            }
                        }
                    }
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
    }
}