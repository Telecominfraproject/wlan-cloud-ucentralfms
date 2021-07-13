//
// Created by stephane bourque on 2021-05-09.
//

#include "StorageService.h"
#include "RESTAPI_handler.h"

#include "RESTAPI_FMSObjects.h"
#include "RESTAPI_utils.h"

namespace uCentral {

    bool Convert(const FirmwaresRecord &T, FMSObjects::Firmware & F) {
        F.id = T.get<0>();
        F.deviceType = T.get<1>();
        F.description = T.get<2>();
        F.revision = T.get<3>();
        F.uri = T.get<4>();
        F.image = T.get<5>();
        F.imageDate = T.get<6>();
        F.size = T.get<7>();
        F.downloadCount = T.get<8>();
        F.firmwareHash = T.get<9>();
        F.owner = T.get<10>();
        F.location = T.get<11>();
        F.uploader = T.get<12>();
        F.digest = T.get<13>();
        F.latest = T.get<14>();
        F.notes = RESTAPI_utils::to_object_array<SecurityObjects::NoteInfo>(T.get<15>());
        F.created = T.get<16>();
        return true;
    }

    bool Convert(const FMSObjects::Firmware & F, FirmwaresRecord & T) {
        T.set<0>(F.id);
        T.set<1>(F.deviceType);
        T.set<2>(F.description);
        T.set<3>(F.revision);
        T.set<4>(F.uri);
        T.set<5>(F.image);
        T.set<6>(F.imageDate);
        T.set<7>(F.size);
        T.set<8>(F.downloadCount);
        T.set<9>(F.firmwareHash);
        T.set<10>(F.owner);
        T.set<11>(F.location);
        T.set<12>(F.uploader);
        T.set<13>(F.digest);
        T.set<14>(F.latest);
        T.set<15>(RESTAPI_utils::to_string(F.notes));
        T.set<16>(F.created);
        return true;
    }

    bool Storage::AddFirmware(FMSObjects::Firmware & F) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Insert(Sess);

            // find the older software and change to latest = 0
            if(F.latest)
            {
                Poco::Data::Statement   Update(Sess);
                std::string st{"UPDATE " + DBNAME_FIRMWARES + " SET latest=0 WHERE deviceType=? AND Latest=1"};
                Update <<   ConvertParams(st),
                            Poco::Data::Keywords::use(F.deviceType);
                Update.execute();
            }

            auto Notes = RESTAPI_utils::to_string(F.notes);
            std::string st{"INSERT INTO " + DBNAME_FIRMWARES + " (" +
                               DBFIELDS_FIRMWARES_SELECT +
                            ") VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"};
            Insert      <<  ConvertParams(st),
                            Poco::Data::Keywords::use(F.id),
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

    bool Storage::GetFirmwares(uint64_t From, uint64_t HowMany, FMSObjects::FirmwareVec & Firmwares) {
        try {
            FirmwaresRecordList      Records;
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string st{"SELECT " + DBFIELDS_FIRMWARES_SELECT +
                           " FROM " + DBNAME_FIRMWARES };

            Select << ConvertParams(st),
                        Poco::Data::Keywords::into(Records),
                        Poco::Data::Keywords::range(From, From + HowMany);
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

}

