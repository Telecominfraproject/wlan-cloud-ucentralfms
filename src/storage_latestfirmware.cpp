//
// Created by stephane bourque on 2021-05-09.
//

#include "uStorageService.h"

namespace uCentral::Storage {

    bool AddLatestFirmware(std::string & DeviceType, std::string &UUID) {
        return Service::instance()->AddLatestFirmware(DeviceType, UUID);
    }

    bool GetLatestFirmware(std::string & DeviceType, std::string &UUID) {
        return Service::instance()->GetLatestFirmware(DeviceType, UUID);
    }

    bool DeleteLatestFirmware(std::string & DeviceType) {
        return Service::instance()->DeleteLatestFirmware(DeviceType);
    }

    bool GetLatestFirmwareList(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::LatestFirmware> & LatestFirmwareList) {
        return Service::instance()->GetLatestFirmwareList(From, HowMany, LatestFirmwareList);
    }

    bool Service::AddLatestFirmware(std::string & DeviceType, std::string &UUID) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::GetLatestFirmware(std::string & DeviceType, std::string &UUID) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::DeleteLatestFirmware(std::string & DeviceType) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::GetLatestFirmwareList(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::LatestFirmware> & LatestFirmwareList) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

}


