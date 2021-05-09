//
// Created by stephane bourque on 2021-05-09.
//

#include "uStorageService.h"

namespace uCentral::Storage {

    bool AddFirmware(uCentral::Objects::Firmware & F) {
        return Service::instance()->AddFirmware(F);
    }

    bool UpdateFirmware(std::string & UUID, uCentral::Objects::Firmware & C) {
        return Service::instance()->UpdateFirmware(UUID, C);
    }

    bool DeleteFirmware(std::string & UUID) {
        return Service::instance()->DeleteFirmware(UUID);
    }

    bool GetFirmware(std::string & UUID, uCentral::Objects::Firmware & C) {
        return Service::instance()->GetFirmware(UUID, C);
    }

    bool GetFirmwares(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Firmware> & Firmwares) {
        return Service::instance()->GetFirmwares(From, HowMany, Firmwares);
    }

    bool Service::AddFirmware(uCentral::Objects::Firmware & F) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::UpdateFirmware(std::string & UUID, uCentral::Objects::Firmware & C) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::DeleteFirmware(std::string & UUID) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::GetFirmware(std::string & UUID, uCentral::Objects::Firmware & C) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::GetFirmwares(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Firmware> & Firmwares) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

}

