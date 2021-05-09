//
// Created by stephane bourque on 2021-05-09.
//

#include "uStorageService.h"

namespace uCentral::Storage {

    bool AddCallback(uCentral::Objects::Callback & C) {
        return Service::instance()->AddCallback(C);
    }

    bool UpdateCallback(std::string & UUID, uCentral::Objects::Callback & C) {
        return Service::instance()->UpdateCallback(UUID, C);
    }

    bool DeleteCallback(std::string & UUID) {
        return Service::instance()->DeleteCallback(UUID);
    }

    bool GetCallback(std::string & UUID, uCentral::Objects::Callback & C) {
        return Service::instance()->GetCallback(UUID, C);
    }

    bool GetCallBacks(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Callback> & Callbacks) {
        return Service::instance()->GetCallBacks(From, HowMany, Callbacks);
    }

    bool Service::AddCallback(uCentral::Objects::Callback & C) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::UpdateCallback(std::string & UUID, uCentral::Objects::Callback & C) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::DeleteCallback(std::string & UUID) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::GetCallback(std::string & UUID, uCentral::Objects::Callback & C) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::GetCallBacks(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Callback> & Callbacks) {
        try {
            Poco::Data::Session Sess = Pool_->get();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }
}
