//
// Created by stephane bourque on 2021-05-09.
//

#include "RESTAPI_firmwaresHandler.h"
#include "LatestFirmwareCache.h"
#include "StorageService.h"
#include "framework/ow_constants.h"

namespace OpenWifi {
	void RESTAPI_firmwaresHandler::DoGet() {
		std::string DeviceType = ORM::Escape(GetParameter(RESTAPI::Protocol::DEVICETYPE, ""));
		bool IdOnly = GetBoolParameter(RESTAPI::Protocol::IDONLY);
		bool LatestOnly = GetBoolParameter(RESTAPI::Protocol::LATESTONLY);
		bool rcOnly = GetBoolParameter("rcOnly");

		if (GetBoolParameter("updateTimeOnly")) {
			Poco::JSON::Object Answer;
			Answer.set("lastUpdateTime", ManifestCreator()->LastUpdate());
			return ReturnObject(Answer);
		}

		if (GetBoolParameter(RESTAPI::Protocol::DEVICESET)) {
			auto Revisions = LatestFirmwareCache()->GetDevices();
			Poco::JSON::Array ObjectArray;
			for (const auto &i : Revisions) {
				ObjectArray.add(i);
			}
			Poco::JSON::Object RetObj;
			RetObj.set(RESTAPI::Protocol::DEVICETYPES, ObjectArray);
			return ReturnObject(RetObj);
		}

		if (GetBoolParameter(RESTAPI::Protocol::REVISIONSET)) {
			auto Revisions = LatestFirmwareCache()->GetRevisions();
			Poco::JSON::Array ObjectArray;
			for (const auto &i : Revisions) {
				ObjectArray.add(i);
			}
			Poco::JSON::Object RetObj;
			RetObj.set(RESTAPI::Protocol::REVISIONS, ObjectArray);
			return ReturnObject(RetObj);
		}

		// special cases: if latestOnly and deviceType
		if (!DeviceType.empty()) {
			if (LatestOnly) {
				LatestFirmwareCacheEntry Entry;
				if (rcOnly) {
					if (!LatestFirmwareCache()->FindLatestRCOnlyFirmware(DeviceType, Entry)) {
						return NotFound();
					}
				} else {
					if (!LatestFirmwareCache()->FindLatestFirmware(DeviceType, Entry)) {
						return NotFound();
					}
				}

				FMSObjects::Firmware F;
				if (StorageService()->FirmwaresDB().GetFirmware(Entry.Id, F)) {
					Poco::JSON::Object Answer;
					F.to_json(Answer);
					return ReturnObject(Answer);
				}
				return NotFound();
			} else {
				std::vector<FMSObjects::Firmware> List;
				if (StorageService()->FirmwaresDB().GetFirmwares(QB_.Offset, QB_.Limit, DeviceType,
																 List)) {
					Poco::JSON::Array ObjectArray;
					for (const auto &i : List) {
						if (rcOnly && !LatestFirmwareCache::IsRC(i.revision))
							continue;
						if (IdOnly) {
							ObjectArray.add(i.id);
						} else {
							Poco::JSON::Object Obj;
							i.to_json(Obj);
							ObjectArray.add(Obj);
						}
					}
					Poco::JSON::Object RetObj;
					RetObj.set(RESTAPI::Protocol::FIRMWARES, ObjectArray);
					return ReturnObject(RetObj);
				} else {
					return NotFound();
				}
			}
		}

		std::vector<FMSObjects::Firmware> List;
		Poco::JSON::Array ObjectArray;
		Poco::JSON::Object Answer;
		if (StorageService()->FirmwaresDB().GetFirmwares(QB_.Offset, QB_.Limit, DeviceType, List)) {
			for (const auto &i : List) {
				if (rcOnly && !LatestFirmwareCache::IsRC(i.revision))
					continue;
				if (IdOnly) {
					ObjectArray.add(i.id);
				} else {
					Poco::JSON::Object Obj;
					i.to_json(Obj);
					ObjectArray.add(Obj);
				}
			}
		}
		Answer.set(RESTAPI::Protocol::FIRMWARES, ObjectArray);
		ReturnObject(Answer);
	}

	void RESTAPI_firmwaresHandler::DoPut() {
		if (UserInfo_.userinfo.userRole != SecurityObjects::ROOT &&
			UserInfo_.userinfo.userRole != SecurityObjects::ADMIN) {
			return UnAuthorized(RESTAPI::Errors::ACCESS_DENIED);
		}

		if (GetBoolParameter("update")) {
			if (ManifestCreator()->RunUpdateTask()) {
				return OK();
			}
			return BadRequest(RESTAPI::Errors::FirmwareBDInProgress);
		}

		return BadRequest(RESTAPI::Errors::MissingOrInvalidParameters);
	}
} // namespace OpenWifi
