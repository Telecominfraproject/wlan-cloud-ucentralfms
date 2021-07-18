//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#include "StorageService.h"

namespace uCentral {

	int Storage::Create_Tables() {
	    Create_Firmwares();
	    Create_History();
	    Create_DeviceTypes();
        Create_DeviceInfo();
		return 0;
	}

	int Storage::Create_Firmwares() {
		try {
			Poco::Data::Session Sess = Pool_->get();

			if(dbType_==mysql) {
                Sess << "CREATE TABLE IF NOT EXISTS " + DBNAME_FIRMWARES + " (" +
                        DBFIELDS_FIRMWARES_CREATION +
                        ")",
                        Poco::Data::Keywords::now;
            } else {
                Sess << "CREATE TABLE IF NOT EXISTS " + DBNAME_FIRMWARES + " (" +
                        DBFIELDS_FIRMWARES_CREATION +
                        ")",
                        Poco::Data::Keywords::now;
			}
			return 0;
		} catch(const Poco::Exception &E) {
			Logger_.log(E);
		}
		return -1;
	}

    int Storage::Create_History() {
        try {
            Poco::Data::Session Sess = Pool_->get();

            if(dbType_==mysql) {
                Sess << "CREATE TABLE IF NOT EXISTS " + DBNAME_HISTORY + " (" +
                        DBFIELDS_HISTORY_CREATION +
                        ",INDEX Serial (SerialNumber ASC, upgraded ASC))"
                        , Poco::Data::Keywords::now;
            } else {
                Sess << "CREATE TABLE IF NOT EXISTS " + DBNAME_HISTORY + " (" +
                        DBFIELDS_HISTORY_CREATION +
                        ")",
                        Poco::Data::Keywords::now;
                Sess << "CREATE INDEX IF NOT EXISTS Serial ON " + DBNAME_HISTORY + " (SerialNumber ASC, upgraded ASC)", Poco::Data::Keywords::now;
            }
            return 0;
        } catch(const Poco::Exception &E) {
            Logger_.log(E);
        }
        return -1;
    }

    int Storage::Create_DeviceTypes() {
        try {
            Poco::Data::Session Sess = Pool_->get();

            if(dbType_==mysql) {
                Sess << "CREATE TABLE IF NOT EXISTS " + DBNAME_DEVICETYPES + " (" +
                        DBFIELDS_DEVICETYPES_CREATION +
                        ")",
                        Poco::Data::Keywords::now;
            } else {
                Sess << "CREATE TABLE IF NOT EXISTS " + DBNAME_DEVICETYPES + " (" +
                        DBFIELDS_DEVICETYPES_CREATION +
                        ")",
                        Poco::Data::Keywords::now;
            }
            return 0;
        } catch(const Poco::Exception &E) {
            Logger_.log(E);
        }
        return -1;
	}

	int Storage::Create_DeviceInfo() {
        try {
            Poco::Data::Session Sess = Pool_->get();

            Sess << "CREATE TABLE IF NOT EXISTS " + DBNAME_DEVICES + " (" +
                    DBFIELDS_DEVICES_CREATION +
                    ")",
                    Poco::Data::Keywords::now;
            return 0;
        } catch(const Poco::Exception &E) {
            Logger_.log(E);
        }

	    return -1;
	}


}

