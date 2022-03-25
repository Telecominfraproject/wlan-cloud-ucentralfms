//
// Created by stephane bourque on 2021-12-29.
//

#pragma once

#include "framework/orm.h"
#include "RESTObjects/RESTAPI_FMSObjects.h"

namespace OpenWifi {
    typedef Poco::Tuple<
            std::string,
            std::string,
            std::string,
            std::string,
            uint64_t,
            std::string> DevicesRecordTuple;
    typedef std::vector<DevicesRecordTuple> DevicesRecordTupleList;

    class DevicesDB : public ORM::DB<DevicesRecordTuple, FMSObjects::DeviceConnectionInformation> {
    public:
        DevicesDB(OpenWifi::DBType T, Poco::Data::SessionPool & P, Poco::Logger &L);
        virtual ~DevicesDB() {};
        bool SetDeviceRevision(std::string &SerialNumber, std::string & Revision, std::string & DeviceType, std::string &EndPoint);
        bool DeleteDevice( std::string & SerialNumber);
        bool SetDeviceDisconnected(std::string &SerialNumber, std::string &EndPoint);
        bool GetDevices(uint64_t From, uint64_t HowMany, std::vector<FMSObjects::DeviceConnectionInformation> & Devices);
        bool GetDevice(std::string &SerialNumber, FMSObjects::DeviceConnectionInformation & Device);
        bool GenerateDeviceReport(FMSObjects::DeviceReport &Report);

    private:
    };

}