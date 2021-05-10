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

/*
        std::string UUID;
        std::string Description;
        std::string FirmwareFileName;
        std::string FirmwareVersion;
        std::string FirmwareHash;
        std::string FirmwareLatestDoc;
        std::string Owner;
        std::string Location;
        std::string DeviceType;
        std::string Uploader;
        std::string Digest;
        std::string S3URI;
        uint64_t    DownloadCount;
        uint64_t    Size;
        uint64_t    Uploaded;
        uint64_t    FirmwareDate;
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
            uint64_t
            >   FirmwareRecordTuple;
    typedef std::vector<FirmwareRecordTuple>  FirmwareRecordList;

    bool Service::AddFirmware(uCentral::Objects::Firmware & F) {
        try {
            Poco::Data::Session     Sess = Pool_->get();
            Poco::Data::Statement   Insert(Sess);

/*
                    "UUID VARCHAR(64) PRIMARY KEY, "
                    "Description VARCHAR(128), "
                    "Owner VARCHAR(128), "
                    "Location TEXT, ",
                    "DeviceType VARCHAR(128), "
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
                    "Size BIGINT, "
 */
            std::string st{"INSERT INTO Firmwares ("
                               "UUID, Description, Owner, Location, DeviceType, Uploader, Digest, "
                               "FirmwareFileName, FirmwareVersion, FirmwareHash, FirmwareLatestDoc, "
                               "S3URI, FirmwareDate, Uploaded, DownloadCount, Size "
                            ") VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"};

            Insert      <<  ConvertParams(st),
                            Poco::Data::Keywords::use(F.UUID),
                            Poco::Data::Keywords::use(F.Description),
                            Poco::Data::Keywords::use(F.Owner),
                            Poco::Data::Keywords::use(F.Location),
                            Poco::Data::Keywords::use(F.DeviceType),
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
                            Poco::Data::Keywords::use(F.Size);
            Insert.execute();
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
                           "Description, Owner, Location, DeviceType, Uploader, Digest, "
                           "FirmwareFileName, FirmwareVersion, FirmwareHash, FirmwareLatestDoc, "
                           "S3URI, FirmwareDate, Uploaded, DownloadCount, Size "
                           " WHERE UUID=?"};

            Update      <<  ConvertParams(st),
                    Poco::Data::Keywords::use(F.Description),
                    Poco::Data::Keywords::use(F.Owner),
                    Poco::Data::Keywords::use(F.Location),
                    Poco::Data::Keywords::use(F.DeviceType),
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
                    Poco::Data::Keywords::use(F.Size);
                    Poco::Data::Keywords::use(F.UUID);

            Update.execute();

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
                           "UUID, Description, Owner, Location, DeviceType, Uploader, Digest, "
                           "FirmwareFileName, FirmwareVersion, FirmwareHash, FirmwareLatestDoc, "
                           "S3URI, FirmwareDate, Uploaded, DownloadCount, Size "
                           " FROM Firmwares WHERE UUID=?"};

            Select      <<  ConvertParams(st),
                    Poco::Data::Keywords::into(F.UUID),
                    Poco::Data::Keywords::into(F.Description),
                    Poco::Data::Keywords::into(F.Owner),
                    Poco::Data::Keywords::into(F.Location),
                    Poco::Data::Keywords::into(F.DeviceType),
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
                    Poco::Data::Keywords::into(F.Size);
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
                           "UUID, Description, Owner, Location, DeviceType, Uploader, Digest, "
                           "FirmwareFileName, FirmwareVersion, FirmwareHash, FirmwareLatestDoc, "
                           "S3URI, FirmwareDate, Uploaded, DownloadCount, Size "
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
                    .DeviceType= i.get<4>(),
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
                    .Size= i.get<15>()
                };
                Firmwares.push_back(F);
            }

            return true;

        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        return false;
    }

}

