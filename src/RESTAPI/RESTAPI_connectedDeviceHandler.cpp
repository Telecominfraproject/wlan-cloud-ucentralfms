//
// Created by stephane bourque on 2021-07-18.
//

#include "RESTAPI_connectedDeviceHandler.h"
#include "RESTObjects/RESTAPI_FMSObjects.h"
#include "StorageService.h"
#include "framework/ow_constants.h"

namespace OpenWifi {

    void RESTAPI_connectedDeviceHandler::DoGet() {
        auto SerialNumber = GetBinding(RESTAPI::Protocol::SERIALNUMBER,"");

        if(SerialNumber.empty()) {
            return BadRequest(RESTAPI::Errors::MissingSerialNumber);
        }

        FMSObjects::DeviceConnectionInformation DevInfo;
        if(StorageService()->DevicesDB().GetDevice(SerialNumber, DevInfo)) {
            Poco::JSON::Object  Answer;
            DevInfo.to_json(Answer);
            return ReturnObject(Answer);
        }
        NotFound();
    }
}