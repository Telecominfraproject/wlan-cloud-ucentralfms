//
// Created by stephane bourque on 2021-07-18.
//

#include "RESTAPI_connectedDeviceHandler.h"
#include "RESTAPI_FMSObjects.h"
#include "StorageService.h"
#include "RESTAPI_protocol.h"
#include "RESTAPI_errors.h"

namespace OpenWifi {

    void RESTAPI_connectedDeviceHandler::DoGet() {
        auto SerialNumber = GetBinding(RESTAPI::Protocol::SERIALNUMBER,"");

        if(SerialNumber.empty()) {
            return BadRequest(RESTAPI::Errors::MissingSerialNumber);
        }

        FMSObjects::DeviceConnectionInformation DevInfo;
        if(Storage()->GetDevice(SerialNumber, DevInfo)) {
            Poco::JSON::Object  Answer;
            DevInfo.to_json(Answer);
            return ReturnObject(Answer);
        }
        NotFound();
    }
}