//
// Created by stephane bourque on 2021-07-13.
//

#include "NewConnectionHandler.h"
#include "Kafka_topics.h"
#include "KafkaManager.h"
#include "OpenWifiTypes.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "StorageService.h"
#include "LatestFirmwareCache.h"
#include "Utils.h"
#include "uCentralProtocol.h"
#include "DeviceCache.h"

/*
{ "system" : { "id" : 6715803232063 , "host" : "https://localhost:17002" } ,
 "payload" : "{"capabilities":{"compatible":"linksys_ea8300","model":"Linksys EA8300 (Dallas)","network":{"lan":["eth0"],"wan":["eth1"]},"platform":"ap","switch":{"switch0":{"enable":true,"ports":[{"device":"eth0","need_tag":false,"num":0,"want_untag":true},{"num":1,"role":"lan"},{"num":2,"role":"lan"},{"num":3,"role":"lan"},{"num":4,"role":"lan"}],"reset":true,"roles":[{"device":"eth0","ports":"1 2 3 4 0","role":"lan"}]}},"wifi":{"platform/soc/a000000.wifi":{"band":["2G"],"channels":[1,2,3,4,5,6,7,8,9,10,11],"frequencies":[2412,2417,2422,2427,2432,2437,2442,2447,2452,2457,2462],"ht_capa":6639,"htmode":["HT20","HT40","VHT20","VHT40","VHT80"],"rx_ant":3,"tx_ant":3,"vht_capa":865687986},"platform/soc/a800000.wifi":{"band":["5G"],"channels":[36,40,44,48,52,56,60,64],"frequencies":[5180,5200,5220,5240,5260,5280,5300,5320],"ht_capa":6639,"htmode":["HT20","HT40","VHT20","VHT40","VHT80"],"rx_ant":3,"tx_ant":3,"vht_capa":865687986},"soc/40000000.pci/pci0000:00/0000:00:00.0/0000:01:00.0":{"band":["5G"],"channels":[100,104,108,112,116,120,124,128,132,136,140,144,149,153,157,161,165],"frequencies":[5500,5520,5540,5560,5580,5600,5620,5640,5660,5680,5700,5720,5745,5765,5785,5805,5825],"ht_capa":6639,"htmode":["HT20","HT40","VHT20","VHT40","VHT80"],"rx_ant":3,"tx_ant":3,"vht_capa":865696178}}},"firmware":"OpenWrt 21.02-SNAPSHOT r16011+53-6fd65c6573 / TIP-devel-0825cb93","serial":"24f5a207a130","uuid":1623866223}}
 */

namespace OpenWifi {
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
                    std::lock_guard G(Mutex_);
                    S = NewConnections_.front();
                    NewConnections_.pop();
                }

                try {
                    auto SerialNumber = S.first;
                    Poco::JSON::Parser  Parser;
                    auto Object = Parser.parse(S.second).extract<Poco::JSON::Object::Ptr>();

                    std::string EndPoint;

                    if(Object->has(uCentralProtocol::SYSTEM)) {
                        auto SystemObj = Object->getObject(uCentralProtocol::SYSTEM);
                        if(SystemObj->has(uCentralProtocol::HOST))
                            EndPoint = SystemObj->get(uCentralProtocol::HOST).toString();
                    }

                    if(Object->has(uCentralProtocol::PAYLOAD)) {
                        auto PayloadObj = Object->getObject(uCentralProtocol::PAYLOAD);
                        if(PayloadObj->has(uCentralProtocol::CAPABILITIES)) {
                            // std::cout << "CAPABILITIES:" << SerialNumber << std::endl;
                            auto CapObj = PayloadObj->getObject(uCentralProtocol::CAPABILITIES);
                            if(CapObj->has(uCentralProtocol::COMPATIBLE)) {
                                auto DeviceType = CapObj->get(uCentralProtocol::COMPATIBLE).toString();
                                auto Serial = PayloadObj->get(uCentralProtocol::SERIAL).toString();
                                auto Revision = Storage::TrimRevision(PayloadObj->get(uCentralProtocol::FIRMWARE).toString());
                                // std::cout << "ConnectionEvent: SerialNumber: " << SerialNumber << " DeviceType: " << DeviceType << " Revision:" << Revision << std::endl;
                                FMSObjects::FirmwareAgeDetails  FA;
                                if(Storage()->ComputeFirmwareAge(DeviceType, Revision, FA)) {
                                    Storage()->SetDeviceRevision(SerialNumber, Revision, DeviceType, EndPoint);
                                    if(FA.age)
                                        Logger_.information(Poco::format("Device %s connection. Firmware is %s older than latest",SerialNumber, Utils::SecondsToNiceText(FA.age)));
                                    else
                                        Logger_.information(Poco::format("Device %s connection. Firmware age cannot be determined",SerialNumber));
                                }
                                DeviceCache()->AddToCache(Serial, DeviceType, EndPoint, Revision);
                            }
                        } else if(PayloadObj->has(uCentralProtocol::DISCONNECTION)) {
                            auto DisconnectMessage = PayloadObj->getObject(uCentralProtocol::DISCONNECTION);
                            if(DisconnectMessage->has(uCentralProtocol::SERIALNUMBER) && DisconnectMessage->has(uCentralProtocol::TIMESTAMP)) {
                                auto SNum = DisconnectMessage->get(uCentralProtocol::SERIALNUMBER).toString();
                                auto Timestamp = DisconnectMessage->get(uCentralProtocol::TIMESTAMP);
                                Storage()->SetDeviceDisconnected(SNum,EndPoint);
                                // std::cout << "DISCONNECTION:" << SerialNumber << std::endl;
                            }
                        } else if(PayloadObj->has(uCentralProtocol::PING)) {
                            // std::cout << "PING:" << SerialNumber << std::endl;
                            auto PingMessage = PayloadObj->getObject(uCentralProtocol::PING);
                            if( PingMessage->has(uCentralProtocol::FIRMWARE) &&
                            PingMessage->has(uCentralProtocol::SERIALNUMBER) &&
                            PingMessage->has(uCentralProtocol::COMPATIBLE)) {
                                auto Revision = Storage::TrimRevision(PingMessage->get(uCentralProtocol::FIRMWARE).toString());
                                auto Serial = PingMessage->get( uCentralProtocol::SERIALNUMBER).toString();
                                auto DeviceType = PingMessage->get( uCentralProtocol::COMPATIBLE).toString();
                                Storage()->SetDeviceRevision(Serial, Revision, DeviceType, EndPoint);
                                DeviceCache()->AddToCache(Serial, DeviceType, EndPoint, Revision);
                            }
                        }
                    }
                } catch (const Poco::Exception &E) {
                    Logger_.log(E);
                }
            }
        }
    };

    int NewConnectionHandler::Start() {
        Types::TopicNotifyFunction F = [this](std::string s1,std::string s2) { this->ConnectionReceived(s1,s2); };
        ConnectionWatcherId_ = KafkaManager()->RegisterTopicWatcher(KafkaTopics::CONNECTION, F);
        Worker_.start(*this);
        return 0;
    };

    void NewConnectionHandler::Stop() {
        KafkaManager()->UnregisterTopicWatcher(KafkaTopics::CONNECTION, ConnectionWatcherId_);
        Running_ = false;
        Worker_.wakeUp();
        Worker_.join();
    };

    bool NewConnectionHandler::Update() {
        Worker_.wakeUp();
        return true;
    }

    void NewConnectionHandler::ConnectionReceived( const std::string & Key, const std::string & Message) {
        std::lock_guard G(Mutex_);
        NewConnections_.push(std::make_pair(Key,Message));
    }
}