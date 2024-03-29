//
// Created by stephane bourque on 2021-07-18.
//

#include "Poco/JSON/Array.h"
#include "Poco/JSON/Object.h"

#include "RESTAPI_connectedDevicesHandler.h"
#include "RESTObjects/RESTAPI_FMSObjects.h"
#include "StorageService.h"
#include "framework/ow_constants.h"

namespace OpenWifi {
	void RESTAPI_connectedDevicesHandler::DoGet() {
		std::vector<FMSObjects::DeviceConnectionInformation> Devices;
		Poco::JSON::Object AnswerObj;
		Poco::JSON::Array AnswerArr;
		if (StorageService()->DevicesDB().GetDevices(QB_.Offset, QB_.Limit, Devices)) {
			for (const auto &i : Devices) {
				Poco::JSON::Object Obj;
				i.to_json(Obj);
				AnswerArr.add(Obj);
			}
			AnswerObj.set(RESTAPI::Protocol::DEVICES, AnswerArr);
			return ReturnObject(AnswerObj);
		}
		AnswerObj.set(RESTAPI::Protocol::DEVICES, AnswerArr);
		ReturnObject(AnswerObj);
	}
} // namespace OpenWifi