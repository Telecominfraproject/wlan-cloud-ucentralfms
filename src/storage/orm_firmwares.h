//
// Created by stephane bourque on 2021-12-28.
//

#pragma once

#include "ManifestCreator.h"
#include "RESTObjects/RESTAPI_FMSObjects.h"
#include "framework/orm.h"

namespace OpenWifi {
	typedef Poco::Tuple<std::string, std::string, std::string, std::string, std::string,
						std::string, std::string, uint64_t, uint64_t, uint64_t, std::string,
						std::string, std::string, std::string, std::string, bool, std::string,
						uint64_t>
		FirmwaresRecordTuple;
	typedef std::vector<FirmwaresRecordTuple> FirmwaresRecordTupleList;

	class FirmwaresDB : public ORM::DB<FirmwaresRecordTuple, FMSObjects::Firmware> {
	  public:
		FirmwaresDB(OpenWifi::DBType T, Poco::Data::SessionPool &P, Poco::Logger &L);
		virtual ~FirmwaresDB(){};

		bool AddFirmware(FMSObjects::Firmware &F);
		bool UpdateFirmware(std::string &ID, FMSObjects::Firmware &F);
		bool DeleteFirmware(std::string &ID);
		void RemoveOldFirmware();
		bool GetFirmware(std::string &ID, FMSObjects::Firmware &F);
		bool GetFirmwareByName(const std::string &Release, const std::string &DeviceType,
							   FMSObjects::Firmware &Firmware);
		bool GetFirmwareByRevision(const std::string &Revision, const std::string &DeviceType,
								   FMSObjects::Firmware &Firmware);
		bool GetFirmwares(uint64_t From, uint64_t HowMany, const std::string &Compatible,
						  FMSObjects::FirmwareVec &Firmwares);
		void PopulateLatestFirmwareCache();
		bool ComputeFirmwareAge(const std::string &DeviceType, const std::string &Revision,
								FMSObjects::FirmwareAgeDetails &AgeDetails);
		uint64_t RemoveOldDBEntriesNotInManifest(const S3BucketContent &Bucket);

	  private:
	};

} // namespace OpenWifi