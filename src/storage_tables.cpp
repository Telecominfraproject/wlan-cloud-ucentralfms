//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#include "uStorageService.h"

namespace uCentral::Storage {

	int Service::Create_Tables() {

	    Create_Firmwares();
		Create_Callbacks();
        Create_LatestFirmwareList();

		return 0;
	}

/*
        uuid:
          type: string
          format: uuid
        description:
          type: string
        uploaded:
          type: string
          format: 'date-time'
        firmwareDate:
          type: string
          format: 'date-time'
        firmwareFileName:
          type: string
        firmwareVersion:
          type: string  #the version the AP will report
        firmwareHash:
          type: string
        firmwareLatestDoc:
          type: string
        owner:
          type: string
        location:
          type: string
          format: uri
        deviceType:
          type: string
        downloadCount:
          type: integer
          format: int64
        uploader:
          type: string
        size:
          type: integer
          format: int64
        digest:
          type: string
        s3uri:
          type: string
 */

	int Service::Create_Firmwares() {
		try {
			Poco::Data::Session Sess = Pool_->get();

            Sess << "CREATE TABLE IF NOT EXISTS Firmwares ("
                    "UUID VARCHAR(64) PRIMARY KEY, "
                    "Description VARCHAR(128), "
                    "Owner VARCHAR(128), "
                    "Location TEXT, "
                    "DeviceType VARCHAR(128), "
                    "Uploader VARCHAR(128), "
                    "Uploaded BIGINT, "
                    "DownloadCount BIGINT, "
                    "Size BIGINT, "
                    "Digest TEXT, "
                    "FirmwareDate BIGINT, "
                    "FirmwareFileName TEXT, "
                    "FirmwareVersion VARCHAR(128), "
                    "FirmwareHash VARCHAR(32), "
                    "FirmwareLatestDoc TEXT, "
                    "S3URI TEXT )",
                Poco::Data::Keywords::now;
			return 0;
		} catch(const Poco::Exception &E) {
			Logger_.log(E);
		}
		return -1;
	}

/*
      properties:
        uuid:
          type: string
          format: uuid
        uri:
          type: string
          format: uri
        location:
          type: string
          format: uri
        token:
          type: string
        creator:
          type: string
        tokenType:
          type: string
        created:
          type: string
          format: 'date-time'
        expires:
          type: string
          format: 'date-time'

 */

	int Service::Create_Callbacks() {
		try {
			Poco::Data::Session Sess = Pool_->get();

            Sess << "CREATE TABLE IF NOT EXISTS Callbacks ("
                    "UUID VARCHAR(64) PRIMARY KEY, "
                    "URI TEXT, "
                    "Location VARCHAR(128),"
                    "Token TEXT,"
                    "TokenType VARCHAR(64), "
                    "Creator VARCHAR(128), "
                    "Created BIGINT, "
                    "Expires BIGINT)",
                    Poco::Data::Keywords::now;
			return 0;
		} catch(const Poco::Exception &E) {
			Logger_.log(E);
		}
		return -1;
	}

    /*
    LatestFirmware:
    type: object
        properties:
            deviceType:
            type: string
        uuid:
            type: string
            format: uuid
        lastUpdated:
            type: string
            format: 'date-time'
    */
    int Service::Create_LatestFirmwareList() {
        try {
            Poco::Data::Session Sess = Pool_->get();

            Sess << "CREATE TABLE IF NOT EXISTS LatestFirmwares ("
                    "DeviceType     VARCHAR(128) PRIMARY KEY, "
                    "UUID           TEXT, "
                    "LastUpdated    BIGINT"
                    ")",
                    Poco::Data::Keywords::now;
            return 0;
        } catch(const Poco::Exception &E) {
            Logger_.log(E);
        }
        return -1;
	}
}

