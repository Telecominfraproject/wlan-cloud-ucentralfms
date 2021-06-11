//
// Created by stephane bourque on 2021-05-09.
//

#include "StorageService.h"
#include "RESTAPI_handler.h"

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

    bool BuildFirmwareManifest(Poco::JSON::Object & Manifest, uint64_t &Version) {
        return Service::instance()->BuildFirmwareManifest(Manifest, Version);
    }

    uint64_t FirmwareVersion() {
        return Service::instance()->FirmwareVersion();
    }


    /*
        std::string UUID;
        std::string Description;
        std::string FirmwareFileName;
        std::string FirmwareVersion;
        std::string FirmwareHash;
        std::string FirmwareLatestDoc;
        std::string Owner;
        std::string Location;
        std::string Compatible;
        std::string Uploader;
        std::string Digest;
        std::string S3URI;
        uint64_t    DownloadCount;
        uint64_t    Size;
        uint64_t    Uploaded;
        uint64_t    FirmwareDate;
        uint64_t    Latest;
 */

    typedef Poco::Tuple<
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            std::string,
            uint64_t,
            uint64_t,
            uint64_t,
            uint64_t,
            uint64_t
            >   FirmwareRecordTuple;
    typedef std::vector<FirmwareRecordTuple>  FirmwareRecordList;

    uint64_t Service::FirmwareVersion() {
        return FirmwareVersion_;
    }

    bool Service::AddFirmware(uCentral::Objects::Firmware & F) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Insert(Sess);

/*
                    "UUID VARCHAR(64) PRIMARY KEY, "
                    "Description VARCHAR(128), "
                    "Owner VARCHAR(128), "
                    "Location TEXT, ",
                    "Compatible VARCHAR(128), "
                    "Uploader VARCHAR(128), "
                    "Digest TEXT, "
                    "FirmwareFileName TEXT, "
                    "FirmwareVersion VARCHAR(128), "
                    "FirmwareHash VARCHAR(32), "
                    "FirmwareLatestDoc TEXT, "
                    "S3URI TEXT "
                    "FirmwareDate BIGINT, "
                    "Uploaded BIGINT, "
                    "DownloadCount BIGINT, "
                    "Size BIGINT,
                    "Latest BIGINT"
 */

            // find the older software and change to latest = 0
            if(F.Latest)
            {
                Poco::Data::Statement   Update(Sess);
                std::string st{"UPDATE Firmwares SET Latest=0 WHERE Compatible=? AND Latest=1"};
                Update <<   ConvertParams(st),
                            Poco::Data::Keywords::use(F.Compatible);
                Update.execute();
            }

            std::string st{"INSERT INTO Firmwares ("
                               "UUID, Description, Owner, Location, Compatible, Uploader, Digest, "
                               "FirmwareFileName, FirmwareVersion, FirmwareHash, FirmwareLatestDoc, "
                               "S3URI, FirmwareDate, Uploaded, DownloadCount, Size, Latest "
                            ") VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"};

            Insert      <<  ConvertParams(st),
                            Poco::Data::Keywords::use(F.UUID),
                            Poco::Data::Keywords::use(F.Description),
                            Poco::Data::Keywords::use(F.Owner),
                            Poco::Data::Keywords::use(F.Location),
                            Poco::Data::Keywords::use(F.Compatible),
                            Poco::Data::Keywords::use(F.Uploader),
                            Poco::Data::Keywords::use(F.Digest),
                            Poco::Data::Keywords::use(F.FirmwareFileName),
                            Poco::Data::Keywords::use(F.FirmwareVersion),
                            Poco::Data::Keywords::use(F.FirmwareHash),
                            Poco::Data::Keywords::use(F.FirmwareLatestDoc),
                            Poco::Data::Keywords::use(F.S3URI),
                            Poco::Data::Keywords::use(F.FirmwareDate),
                            Poco::Data::Keywords::use(F.Uploaded),
                            Poco::Data::Keywords::use(F.DownloadCount),
                            Poco::Data::Keywords::use(F.Size),
                            Poco::Data::Keywords::use(F.Latest);
            Insert.execute();
            FirmwareVersion_++;
            return true;

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::UpdateFirmware(std::string & UUID, uCentral::Objects::Firmware & F) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Update(Sess);

            std::string st{"UPDATE Firmwares "
                           " Description, Owner, Location, Compatible, Uploader, Digest, "
                           " FirmwareFileName, FirmwareVersion, FirmwareHash, FirmwareLatestDoc, "
                           " S3URI, FirmwareDate, Uploaded, DownloadCount, Size"
                           " WHERE UUID=?"};

            Update      <<  ConvertParams(st),
                    Poco::Data::Keywords::use(F.Description),
                    Poco::Data::Keywords::use(F.Owner),
                    Poco::Data::Keywords::use(F.Location),
                    Poco::Data::Keywords::use(F.Compatible),
                    Poco::Data::Keywords::use(F.Uploader),
                    Poco::Data::Keywords::use(F.Digest),
                    Poco::Data::Keywords::use(F.FirmwareFileName),
                    Poco::Data::Keywords::use(F.FirmwareVersion),
                    Poco::Data::Keywords::use(F.FirmwareHash),
                    Poco::Data::Keywords::use(F.FirmwareLatestDoc),
                    Poco::Data::Keywords::use(F.S3URI),
                    Poco::Data::Keywords::use(F.FirmwareDate),
                    Poco::Data::Keywords::use(F.Uploaded),
                    Poco::Data::Keywords::use(F.DownloadCount),
                    Poco::Data::Keywords::use(F.Size),
                    Poco::Data::Keywords::use(F.UUID);
            Update.execute();
            FirmwareVersion_++;
            return true;

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::DeleteFirmware(std::string & UUID) {
        try {

            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Delete(Sess);

            std::string st{"DELETE FROM Firmwares WHERE UUID=?"};
            Delete <<   ConvertParams(st),
                        Poco::Data::Keywords::use(UUID);
            Delete.execute();
            FirmwareVersion_++;
            return true;

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::GetFirmware(std::string & UUID, uCentral::Objects::Firmware & F) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string st{"SELECT "
                           "UUID, Description, Owner, Location, Compatible, Uploader, Digest, "
                           "FirmwareFileName, FirmwareVersion, FirmwareHash, FirmwareLatestDoc, "
                           "S3URI, FirmwareDate, Uploaded, DownloadCount, Size, Latest "
                           " FROM Firmwares WHERE UUID=?"};

            Select      <<  ConvertParams(st),
                        Poco::Data::Keywords::into(F.UUID),
                        Poco::Data::Keywords::into(F.Description),
                        Poco::Data::Keywords::into(F.Owner),
                        Poco::Data::Keywords::into(F.Location),
                        Poco::Data::Keywords::into(F.Compatible),
                        Poco::Data::Keywords::into(F.Uploader),
                        Poco::Data::Keywords::into(F.Digest),
                        Poco::Data::Keywords::into(F.FirmwareFileName),
                        Poco::Data::Keywords::into(F.FirmwareVersion),
                        Poco::Data::Keywords::into(F.FirmwareHash),
                        Poco::Data::Keywords::into(F.FirmwareLatestDoc),
                        Poco::Data::Keywords::into(F.S3URI),
                        Poco::Data::Keywords::into(F.FirmwareDate),
                        Poco::Data::Keywords::into(F.Uploaded),
                        Poco::Data::Keywords::into(F.DownloadCount),
                        Poco::Data::Keywords::into(F.Size),
                        Poco::Data::Keywords::into(F.Latest),
                        Poco::Data::Keywords::use(UUID);
            Select.execute();

            return !F.UUID.empty();

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

    bool Service::GetFirmwares(uint64_t From, uint64_t HowMany, std::vector<uCentral::Objects::Firmware> & Firmwares) {
        try {
            FirmwareRecordList      Records;
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string st{"SELECT "
                           "UUID, Description, Owner, Location, Compatible, Uploader, Digest, "
                           "FirmwareFileName, FirmwareVersion, FirmwareHash, FirmwareLatestDoc, "
                           "S3URI, FirmwareDate, Uploaded, DownloadCount, Size, Latest "
                           " FROM Firmwares"};

            Select << ConvertParams(st),
                        Poco::Data::Keywords::into(Records),
                        Poco::Data::Keywords::range(From, From + HowMany);
            Select.execute();

            for(auto const &i:Records) {
                uCentral::Objects::Firmware F{
                    .UUID = i.get<0>(),
                    .Description= i.get<1>(),
                    .Owner= i.get<2>(),
                    .Location= i.get<3>(),
                    .Compatible= i.get<4>(),
                    .Uploader= i.get<5>(),
                    .Digest= i.get<6>(),
                    .FirmwareFileName= i.get<7>(),
                    .FirmwareVersion= i.get<8>(),
                    .FirmwareHash= i.get<9>(),
                    .FirmwareLatestDoc= i.get<10>(),
                    .S3URI= i.get<11>(),
                    .FirmwareDate= i.get<12>(),
                    .Uploaded= i.get<13>(),
                    .DownloadCount= i.get<14>(),
                    .Size= i.get<15>(),
                    .Latest = (bool) (i.get<16>()!=0)
                };
                Firmwares.push_back(F);
            }

            return true;

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }


/*
    "Compatible VARCHAR(128), "
    "Uploader VARCHAR(128), "
    "FirmwareVersion VARCHAR(128), "
    "S3URI TEXT )",
    "Uploaded BIGINT, "
    "Size BIGINT, "
    "FirmwareDate BIGINT, "
 */

    typedef Poco::Tuple<
            std::string,
            std::string,
            std::string,
            std::string,
            uint64_t,
            uint64_t,
            uint64_t,
            uint64_t
    >   FirmwareManifestTuple;
    typedef std::vector<FirmwareManifestTuple>  FirmwareManifestList;


    bool Service::BuildFirmwareManifest(Poco::JSON::Object & Manifest, uint64_t & Version) {
        try {
            SubMutexGuard           Guard(Mutex_);
            FirmwareManifestList    Records;
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Select(Sess);

            std::string st{"SELECT Compatible,Uploader,FirmwareVersion,S3URI,Uploaded,Size,FirmwareDate,Latest FROM Firmwares"};

            Select <<   ConvertParams(st),
                        Poco::Data::Keywords::into(Records);
            Select.execute();

            Poco::JSON::Array   Elements;
            for(const auto &i:Records) {
                Poco::JSON::Object  Obj;

                Obj.set("compatible", i.get<0>());
                Obj.set("uploader",i.get<1>());
                Obj.set("version",i.get<2>());
                Obj.set("uri",i.get<3>());
                Obj.set("uploaded",RESTAPIHandler::to_RFC3339(i.get<4>()));
                Obj.set("size",i.get<5>());
                Obj.set("date", RESTAPIHandler::to_RFC3339(i.get<6>()));
                Obj.set("latest", (bool) (i.get<7>() != 0));

                Elements.add(Obj);
            }

            Manifest.set("firmwares",Elements);
            Version = FirmwareVersion_;

            return true;

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }


}

