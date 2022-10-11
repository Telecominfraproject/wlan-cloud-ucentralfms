//
// Created by stephane bourque on 2021-12-28.
//

#include "orm_firmwares.h"
#include "LatestFirmwareCache.h"
#include "ManifestCreator.h"

#include "framework/RESTAPI_utils.h"
#include "framework/MicroServiceFuncs.h"

/*
            "Id              varchar(36) UNIQUE PRIMARY KEY, "
            "release            varchar, "
            "deviceType         varchar, "
            "description        varchar, "
            "revision           varchar, "
            "uri                varchar, "
            "image              varchar, "
            "imageDate          bigint, "
            "size               bigint, "
            "downloadCount      bigint, "
            "firmwareHash       varchar, "
            "owner              varchar, "
            "location           varchar, "
            "uploader           varchar, "
            "digest             varchar, "
            "latest             boolean,  "
            "notes              text, "
            "created            bigint"
 */

namespace OpenWifi {

    static ORM::FieldVec FirmwaresDB_Fields{
            ORM::Field{"id", 36, true},
            ORM::Field{"release", ORM::FieldType::FT_TEXT},
            ORM::Field{"deviceType", ORM::FieldType::FT_TEXT},
            ORM::Field{"description", ORM::FieldType::FT_TEXT},
            ORM::Field{"revision", ORM::FieldType::FT_TEXT},
            ORM::Field{"uri", ORM::FieldType::FT_TEXT},
            ORM::Field{"image", ORM::FieldType::FT_TEXT},
            ORM::Field{"imageDate", ORM::FieldType::FT_BIGINT},
            ORM::Field{"size", ORM::FieldType::FT_BIGINT},
            ORM::Field{"downloadCount", ORM::FieldType::FT_BIGINT},
            ORM::Field{"firmwareHash", ORM::FieldType::FT_TEXT},
            ORM::Field{"owner", ORM::FieldType::FT_TEXT},
            ORM::Field{"location", ORM::FieldType::FT_TEXT},
            ORM::Field{"uploader", ORM::FieldType::FT_TEXT},
            ORM::Field{"digest", ORM::FieldType::FT_TEXT},
            ORM::Field{"latest", ORM::FieldType::FT_BOOLEAN},
            ORM::Field{"notes", ORM::FieldType::FT_TEXT},
            ORM::Field{"created", ORM::FieldType::FT_BIGINT}
    };

    FirmwaresDB::FirmwaresDB(OpenWifi::DBType T,
                         Poco::Data::SessionPool &P, Poco::Logger &L) :
            DB(T, "firmwares", FirmwaresDB_Fields,{}, P, L, "fws") {
    }

    bool FirmwaresDB::AddFirmware(FMSObjects::Firmware & F) {
        // find the older software and change to latest = 0
        F.id = MicroServiceCreateUUID();
        if (LatestFirmwareCache()->AddToCache(F.deviceType, F.revision, F.id, F.imageDate)) {
            F.latest = true;
            std::vector<FMSObjects::Firmware> Fs;
            std::string WhereClause{" deviceType='" + F.deviceType + "' AND Latest=true "};
            if (GetRecords(0, 200, Fs, WhereClause)) {
                for (auto &i: Fs) {
                    i.latest = false;
                    UpdateRecord("id", i.id, i);
                }
            }
        } else {
            F.latest = false;
        }

        return CreateRecord(F);
    }

    bool FirmwaresDB::UpdateFirmware(std::string & Id, FMSObjects::Firmware & F) {
        return UpdateRecord("id",Id, F);
    }

    bool FirmwaresDB::DeleteFirmware(std::string & Id) {
        return DeleteRecord("id", Id);
    }

    void FirmwaresDB::RemoveOldFirmware() {
        uint64_t Limit = OpenWifi::Now() - ManifestCreator()->MaxAge();
        std::string WhereClause{"imageDate < " + std::to_string(Limit)};
        DeleteRecords(WhereClause);
    }

    bool FirmwaresDB::GetFirmware(std::string & Id, FMSObjects::Firmware & F) {
        return GetRecord("id", Id, F);
    }

    bool FirmwaresDB::GetFirmwareByName(const std::string & Release, const std::string &DeviceType, FMSObjects::Firmware & Firmware ) {
        std::string WhereClause{" release='" + Release + "' and DeviceType='" + DeviceType + "' "};
        std::vector<FMSObjects::Firmware>   Fs;
        if(GetRecords(0,2,Fs,WhereClause)) {
            Firmware = Fs[0];
            return true;
        }
        return false;
    }

    bool FirmwaresDB::GetFirmwareByRevision(const std::string & Revision, const std::string &DeviceType,FMSObjects::Firmware & Firmware ) {
        std::string WhereClause{" Revision='" + Revision + "' and DeviceType='" + DeviceType + "' "};
        std::vector<FMSObjects::Firmware> Fs;
        if (GetRecords(0, 2, Fs, WhereClause)) {
            Firmware = Fs[0];
            return true;
        }
        return false;
    }

    bool FirmwaresDB::GetFirmwares(uint64_t From, uint64_t HowMany, const std::string & Compatible, FMSObjects::FirmwareVec & Firmwares) {
        if(Compatible.empty()) {
            GetRecords(From, HowMany, Firmwares);
            return true;
        } else {
            std::string WhereClause{ " DeviceType='" + Compatible + "' "};
            GetRecords(From, HowMany, Firmwares, WhereClause);
            return true;
        }
    }

    void FirmwaresDB::PopulateLatestFirmwareCache() {
        Iterate([](const OpenWifi::FMSObjects::Firmware &F){
            LatestFirmwareCache()->AddToCache(F.deviceType, F.revision, F.id, F.imageDate);
            return true;
        });
    }

    bool FirmwaresDB::ComputeFirmwareAge(const std::string & DeviceType, const std::string & Revision, FMSObjects::FirmwareAgeDetails &AgeDetails) {
        try {
            FMSObjects::Firmware    CurrentFirmware;
            FMSObjects::Firmware    LatestFirmware;
            bool CurrentFirmwareExists = false;

            if(GetFirmwareByRevision(Revision,DeviceType,CurrentFirmware)) {
                CurrentFirmwareExists = true;
            }

            LatestFirmwareCacheEntry    LE;
            if(LatestFirmwareCache()->FindLatestFirmware(DeviceType,LE)) {
                // std::cout << "LE.id" << LE.Id << std::endl;

                if(GetFirmware(LE.Id, LatestFirmware)) {
                    AgeDetails.imageDate = LatestFirmware.imageDate;
                    AgeDetails.uri = LatestFirmware.uri;
                    AgeDetails.image = LatestFirmware.image;
                    AgeDetails.revision = LatestFirmware.revision;
                    AgeDetails.latestId = LatestFirmware.id;
                    // std::cout << "   Revision='" << Revision << "'" << std::endl;
                    // std::cout << "LF Revision='" << LatestFirmware.revision << "'" << std::endl;

                    AgeDetails.latest = (Revision == LatestFirmware.revision);
                    // std::cout << "Latest=" << AgeDetails.latest << std::endl;
                    AgeDetails.age = CurrentFirmwareExists ? (LatestFirmware.imageDate - CurrentFirmware.imageDate) : 0;
                    //std::cout << "Revision: '" << Revision << "' vs '" << LatestFirmware.revision << "'" << std::endl;
                    //if (AgeDetails.latest)
                    //    std::cout << "Found latest firmware" << std::endl;
                    return true;
                } else {
                    // std::cout << "Cannot find firmware: " << LE.Id << std::endl;
                }
            }
        } catch (const Poco::Exception &E) {
            Logger().log(E);
        }
        return false;
    }

}

template<> void ORM::DB<OpenWifi::FirmwaresRecordTuple, OpenWifi::FMSObjects::Firmware>::Convert(const OpenWifi::FirmwaresRecordTuple &T, OpenWifi::FMSObjects::Firmware &F ) {
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
    F.notes = OpenWifi::RESTAPI_utils::to_object_array<OpenWifi::SecurityObjects::NoteInfo>(T.get<16>());
    F.created = T.get<17>();
}

template<> void ORM::DB<OpenWifi::FirmwaresRecordTuple, OpenWifi::FMSObjects::Firmware>::Convert(const OpenWifi::FMSObjects::Firmware &F, OpenWifi::FirmwaresRecordTuple &T ) {
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
    T.set<16>(OpenWifi::RESTAPI_utils::to_string(F.notes));
    T.set<17>(F.created);
}
