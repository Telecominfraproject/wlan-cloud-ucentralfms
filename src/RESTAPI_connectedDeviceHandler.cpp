//
// Created by stephane bourque on 2021-07-18.
//

#include "RESTAPI_connectedDeviceHandler.h"
#include "RESTAPI_FMSObjects.h"
#include "StorageService.h"
#include "RESTAPI_protocol.h"

namespace OpenWifi {

    void RESTAPI_connectedDeviceHandler::DoGet() {
        auto SerialNumber = GetBinding(RESTAPI::Protocol::SERIALNUMBER,"");

        if(SerialNumber.empty()) {
            BadRequest("SerialNumber must be specified.");
            return;
        }

        FMSObjects::DeviceConnectionInformation DevInfo;
        if(Storage()->GetDevice(SerialNumber, DevInfo)) {
            Poco::JSON::Object  Answer;
            DevInfo.to_json(Answer);
            ReturnObject(Answer);
            return;
        }
        NotFound();
    }
}