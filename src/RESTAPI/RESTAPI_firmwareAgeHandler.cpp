//
// Created by stephane bourque on 2021-07-16.
//

#include "RESTAPI_firmwareAgeHandler.h"

#include "DeviceCache.h"
#include "Poco/JSON/Parser.h"
#include "StorageService.h"
#include "framework/ow_constants.h"

namespace OpenWifi {
	void RESTAPI_firmwareAgeHandler::DoGet() {
		if (!QB_.Select.empty()) {
			Poco::JSON::Array Objects;
			for (auto &i : SelectedRecords()) {
				DeviceCacheEntry E;
				if (DeviceCache()->GetDevice(i, E)) {
					FMSObjects::FirmwareAgeDetails FA;
					if (StorageService()->FirmwaresDB().ComputeFirmwareAge(E.deviceType, E.revision,
																		   FA)) {
						Poco::JSON::Object O;
						FA.to_json(O);
						O.set(uCentralProtocol::SERIALNUMBER, i);
						Objects.add(O);
					} else {
						Poco::JSON::Object O;
						O.set(uCentralProtocol::SERIALNUMBER, i);
						Objects.add(O);
					}
				} else {
					Poco::JSON::Object O;
					O.set(uCentralProtocol::SERIALNUMBER, i);
					Objects.add(O);
				}
			}
			Poco::JSON::Object Answer;
			Answer.set(RESTAPI::Protocol::AGES, Objects);
			return ReturnObject(Answer);
		} else {
			auto DeviceType = ORM::Escape(GetParameter(RESTAPI::Protocol::DEVICETYPE, ""));
			auto Revision = ORM::Escape(GetParameter(RESTAPI::Protocol::REVISION, ""));

			if (DeviceType.empty() || Revision.empty()) {
				return BadRequest(RESTAPI::Errors::BothDeviceTypeRevision);
			}

			Revision = Storage::TrimRevision(Revision);

			FMSObjects::FirmwareAgeDetails FA;
			if (StorageService()->FirmwaresDB().ComputeFirmwareAge(DeviceType, Revision, FA)) {
				Poco::JSON::Object Answer;

				FA.to_json(Answer);
				return ReturnObject(Answer);
			}
			NotFound();
		}
	}
} // namespace OpenWifi