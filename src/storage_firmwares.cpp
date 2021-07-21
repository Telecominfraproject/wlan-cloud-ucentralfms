//
// Created by stephane bourque on 2021-05-09.
//

#include "StorageService.h"

#include "RESTAPI_FMSObjects.h"
#include "RESTAPI_utils.h"
#include "LatestFirmwareCache.h"
#include "Daemon.h"
#include <limits>

namespace uCentral {

    bool Convert(const FirmwaresRecord &T, FMSObjects::Firmware & F) {
        F.id = T.get<0>();
        F.release = T.get<1>();
        F.deviceType = T.get<2>();
        F.description = T.get<3>();
        F.revision = T.get<4>();
        F.uri = T.get<5>();
        F.image = T.get<6>();
        F.imageDate = T.get<7>();
        F.size = T.get<8>();
        F.downloadCount = T.get<9>();
        F.firmwareHash = T.get<10>();
        F.owner = T.get<11>();
        F.location = T.get<12>();
        F.uploader = T.get<13>();
        F.digest = T.get<14>();
        F.latest = T.get<15>();
        F.notes = RESTAPI_utils::to_object_array<SecurityObjects::NoteInfo>(T.get<16>());
        F.created = T.get<17>();
        return true;
    }

    bool Convert(const FMSObjects::Firmware & F, FirmwaresRecord & T) {
        T.set<0>(F.id);
        T.set<1>(F.release);
        T.set<2>(F.deviceType);
        T.set<3>(F.description);
        T.set<4>(F.revision);
        T.set<5>(F.uri);
        T.set<6>(F.image);
        T.set<7>(F.imageDate);
        T.set<8>(F.size);
        T.set<9>(F.downloadCount);
        T.set<10>(F.firmwareHash);
        T.set<11>(F.owner);
        T.set<12>(F.location);
        T.set<13>(F.uploader);
        T.set<14>(F.digest);
        T.set<15>(F.latest);
        T.set<16>(RESTAPI_utils::to_string(F.notes));
        T.set<17>(F.created);
        return true;
    }

    bool Storage::AddFirmware(FMSObjects::Firmware & F) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Insert(Sess);

            // find the older software and change to latest = 0
            F.id = Daemon()->CreateUUID();
            if(F.latest)
            {
                Poco::Data::Statement   Update(Sess);
                std::string st{"UPDATE " + DBNAME_FIRMWARES + " SET latest=0 WHERE deviceType=? AND Latest=1"};
                Update <<   ConvertParams(st),
                            Poco::Data::Keywords::use(F.deviceType);
                Update.execute();
            }

            LatestFirmwareCache()->AddToCache(F.deviceType,F.revision,F.id,F.imageDate);

            auto Notes = RESTAPI_utils::to_string(F.notes);
            std::string st{"INSERT INTO " + DBNAME_FIRMWARES + " (" +
                               DBFIELDS_FIRMWARES_SELECT +
                            ") VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"};
            Insert      <<  ConvertParams(st),
                            Poco::Data::Keywords::use(F.id),
                            Poco::Data::Keywords::use(F.release),
                            Poco::Data::Keywords::use(F.deviceType),
                            Poco::Data::Keywords::use(F.description),
                            Poco::Data::Keywords::use(F.revision),
                            Poco::Data::Keywords::use(F.uri),
                            Poco::Data::Keywords::use(F.image),
                            Poco::Data::Keywords::use(F.imageDate),
                            Poco::Data::Keywords::use(F.size),
                            Poco::Data::Keywords::use(F.downloadCount),
                            Poco::Data::Keywords::use(F.firmwareHash),
                            Poco::Data::Keywords::use(F.owner),
                            Poco::Data::Keywords::use(F.location),
                            Poco::Data::Keywords::use(F.uploader),
                            Poco::Data::Keywords::use(F.digest),
                            Poco::Data::Keywords::use(F.latest),
                            Poco::Data::Keywords::use(Notes),
                            Poco::Data::Keywords::use(F.created);
            Insert.execute();
            return true;

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::UpdateFirmware(std::string & ID, FMSObjects::Firmware & F) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Update(Sess);

            std::string st{"UPDATE " + DBNAME_FIRMWARES + " set " + DBFIELDS_DEVICETYPES_UPDATE +
                           " WHERE id=?"};
            auto Notes = RESTAPI_utils::to_string(F.notes);

            Update  <<  ConvertParams(st),
                    Poco::Data::Keywords::use(F.id),
                    Poco::Data::Keywords::use(F.release),
                    Poco::Data::Keywords::use(F.deviceType),
                    Poco::Data::Keywords::use(F.description),
                    Poco::Data::Keywords::use(F.revision),
                    Poco::Data::Keywords::use(F.uri),
                    Poco::Data::Keywords::use(F.image),
                    Poco::Data::Keywords::use(F.imageDate),
                    Poco::Data::Keywords::use(F.size),
                    Poco::Data::Keywords::use(F.downloadCount),
                    Poco::Data::Keywords::use(F.firmwareHash),
                    Poco::Data::Keywords::use(F.owner),
                    Poco::Data::Keywords::use(F.location),
                    Poco::Data::Keywords::use(F.uploader),
                    Poco::Data::Keywords::use(F.digest),
                    Poco::Data::Keywords::use(F.latest),
                    Poco::Data::Keywords::use(Notes),
                    Poco::Data::Keywords::use(F.created);
                    Poco::Data::Keywords::use(ID);
            Update.execute();
            return true;

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::DeleteFirmware(std::string & ID) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Delete(Sess);

            std::string st{"DELETE FROM " + DBNAME_FIRMWARES + " WHERE id=?"};
            Delete <<   ConvertParams(st),
                        Poco::Data::Keywords::use(ID);
            Delete.execute();
            return true;

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::GetFirmware(std::string & ID, FMSObjects::Firmware & F) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string st{"SELECT " + DBFIELDS_FIRMWARES_SELECT +
                           " FROM " + DBNAME_FIRMWARES + " WHERE id=?"};

            FirmwaresRecordList Records;
            Select      <<  ConvertParams(st),
                        Poco::Data::Keywords::into(Records),
                        Poco::Data::Keywords::use(ID);
            Select.execute();

            if(Records.empty())
                return false;

            Convert(Records[0],F);
            return true;

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::GetFirmwareByName( std::string & Release, std::string &DeviceType, FMSObjects::Firmware & Firmware ) {
        try {
            FirmwaresRecordList      Records;
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string st{"SELECT " + DBFIELDS_FIRMWARES_SELECT +
                           " FROM " + DBNAME_FIRMWARES + " where release=? and DeviceType=?"};

            Select << ConvertParams(st),
                    Poco::Data::Keywords::into(Records),
                    Poco::Data::Keywords::use(Release),
                    Poco::Data::Keywords::use(DeviceType);
            Select.execute();

            if(Records.empty())
                return false;
            Convert(Records[0],Firmware);
            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::GetFirmwareByRevision(std::string & Revision, std::string &DeviceType,FMSObjects::Firmware & Firmware ) {
        try {
            FirmwaresRecordList     Records;
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            //  try the full revision...
            try {
                std::string st{"SELECT " + DBFIELDS_FIRMWARES_SELECT +
                               " FROM " + DBNAME_FIRMWARES + " where Revision=? and DeviceType=?"};

                Select << ConvertParams(st),
                        Poco::Data::Keywords::into(Records),
                        Poco::Data::Keywords::use(Revision),
                        Poco::Data::Keywords::use(DeviceType);
                Select.execute();

                if (Records.empty())
                    return false;
                Convert(Records[0], Firmware);
                return true;
            } catch (...) {

            }

            //  try one digit less...
            auto NRev = Revision.substr(0,Revision.size()-1);
            try {
                std::string st{"SELECT " + DBFIELDS_FIRMWARES_SELECT +
                               " FROM " + DBNAME_FIRMWARES + " where Revision=? and DeviceType=?"};

                Select << ConvertParams(st),
                        Poco::Data::Keywords::into(Records),
                        Poco::Data::Keywords::use(NRev),
                        Poco::Data::Keywords::use(DeviceType);
                Select.execute();

                if (Records.empty())
                    return false;
                Convert(Records[0], Firmware);
                return true;
            } catch (...) {

            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Storage::GetFirmwares(uint64_t From, uint64_t HowMany, std::string & Compatible, FMSObjects::FirmwareVec & Firmwares) {
        try {
            FirmwaresRecordList      Records;
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string st;

            if(Compatible.empty()) {
                st = "SELECT " + DBFIELDS_FIRMWARES_SELECT + " FROM " + DBNAME_FIRMWARES;
                Select << ConvertParams(st),
                        Poco::Data::Keywords::into(Records),
                        Poco::Data::Keywords::range(From, From + HowMany);
            } else {
                st = "SELECT " + DBFIELDS_FIRMWARES_SELECT + " FROM " + DBNAME_FIRMWARES + " where DeviceType=?";
                Select << ConvertParams(st),
                        Poco::Data::Keywords::into(Records),
                        Poco::Data::Keywords::use(Compatible),
                        Poco::Data::Keywords::range(From, From + HowMany);
            }
            Select.execute();

            for(const auto &R:Records) {
                FMSObjects::Firmware   F;
                Convert(R,F);
                Firmwares.push_back(F);
            }
            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    void Storage::PopulateLatestFirmwareCache() {
        try {
            typedef Poco::Tuple<
                std::string,
                std::string,
                uint64_t,
                std::string> FCE;
            typedef std::vector<FCE>    FCEList;

            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string st{"SELECT Id, DeviceType, ImageDate, Revision FROM " + DBNAME_FIRMWARES};
            FCEList Records;
            Select << ConvertParams(st),
                    Poco::Data::Keywords::into(Records);
            Select.execute();

            for(const auto &R:Records) {
                LatestFirmwareCache()->AddToCache(R.get<1>(), R.get<3>(), R.get<0>(), R.get<2>());
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
    }

    bool Storage::CompareRevision( const std::string &R1, const std::string &R2) {
        if(R1.size()>10 && R2.size()>10) {
            if (R1.size() == R2.size())
                return R1 == R2;
            if (R1.size() < (R2.size() - 1))
                return R1 == R2.substr(0, R2.size() - 1);
            return R1.substr(0, R1.size() - 1) == R2;
        }
        return false;
    }

    bool Storage::ComputeFirmwareAge(std::string & DeviceType, std::string & Revision, FMSObjects::FirmwareAgeDetails &AgeDetails) {
        try {
            FMSObjects::Firmware    CurrentFirmware;
            FMSObjects::Firmware    LatestFirmware;
            bool CurrentFirmwareExists = false;

            if(GetFirmwareByRevision(Revision,DeviceType,CurrentFirmware)) {
                CurrentFirmwareExists = true;
                std::cout << "Firmware exists in DB: " << std::endl;
            } else {
                std::cout << "Firmware does not exist in DB: " << std::endl;
            }


            LatestFirmwareCacheEntry    LE;
            if(LatestFirmwareCache()->FindLatestFirmware(DeviceType,LE)) {
                GetFirmware(LE.Id,LatestFirmware);
            }

            AgeDetails.imageDate = LatestFirmware.imageDate;
            AgeDetails.uri = LatestFirmware.uri;
            AgeDetails.image = LatestFirmware.image;
            AgeDetails.revision = LatestFirmware.revision;
            AgeDetails.latestId = LatestFirmware.id;
            AgeDetails.latest = Storage::CompareRevision(LatestFirmware.revision,Revision);
            AgeDetails.age = CurrentFirmwareExists ? (LatestFirmware.imageDate-CurrentFirmware.imageDate) : 0;
            std::cout << "Revision: '" << Revision << "' vs '" << LatestFirmware.revision << "'" << std::endl;
            if(AgeDetails.latest)
                std::cout << "Found latest firmware" << std::endl;
            return true;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }

        return false;
    }


}

