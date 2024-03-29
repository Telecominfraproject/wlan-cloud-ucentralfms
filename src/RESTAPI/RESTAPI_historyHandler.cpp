//
// Created by stephane bourque on 2021-07-13.
//

#include "RESTAPI_historyHandler.h"
#include "StorageService.h"
#include "framework/ow_constants.h"

namespace OpenWifi {
	void RESTAPI_historyHandler::DoGet() {
		auto SerialNumber = ORM::Escape(GetBinding(RESTAPI::Protocol::SERIALNUMBER, ""));

		if (SerialNumber.empty()) {
			return BadRequest(RESTAPI::Errors::MissingSerialNumber);
		}

		auto unknownList = GetBoolParameter("unknownList");
		if (SerialNumber == "000000000000" && unknownList) {
			// so let's get all the devices, filter the latest record
			FMSObjects::DeviceCurrentInfoList L;
			StorageService()->HistoryDB().GetUnknownDeviceFirmwares(QB_.Offset, QB_.Limit,
																	L.devices);
			Poco::JSON::Object Answer;
			L.to_json(Answer);
			return ReturnObject(Answer);
		}

		auto currentList = GetBoolParameter("currentList");
		if (SerialNumber == "000000000000" && currentList) {
			// so let's get all the devices, filter the latest record
			FMSObjects::DeviceCurrentInfoList L;
			StorageService()->HistoryDB().GetDeviceFirmwares(QB_.Offset, QB_.Limit, L.devices);
			Poco::JSON::Object Answer;
			L.to_json(Answer);
			return ReturnObject(Answer);
		}

		FMSObjects::RevisionHistoryEntryVec H;
		if (StorageService()->HistoryDB().GetHistory(SerialNumber, QB_.Offset, QB_.Limit, H)) {
			Poco::JSON::Array A;
			for (auto const &i : H) {
				Poco::JSON::Object O;
				i.to_json(O);
				A.add(O);
			}
			Poco::JSON::Object Answer;
			Answer.set(RESTAPI::Protocol::HISTORY, A);
			return ReturnObject(Answer);
		}
		NotFound();
	}

	void RESTAPI_historyHandler::DoDelete() {
		auto SerialNumber = GetBinding(RESTAPI::Protocol::SERIALNUMBER, "");
		auto Id = GetParameter(RESTAPI::Protocol::ID, "");
		if (SerialNumber.empty() || Id.empty()) {
			return BadRequest(RESTAPI::Errors::IdOrSerialEmpty);
		}

		if (!StorageService()->HistoryDB().DeleteHistory(SerialNumber, Id)) {
			return OK();
		}
		NotFound();
	}
} // namespace OpenWifi