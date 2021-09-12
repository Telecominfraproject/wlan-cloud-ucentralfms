//
// Created by stephane bourque on 2021-07-18.
//

#include "RESTAPI_connectedDevicesHandler.h"
#include "RESTAPI_FMSObjects.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Array.h"
#include "StorageService.h"
#include "RESTAPI_protocol.h"

namespace OpenWifi {
    void RESTAPI_connectedDevicesHandler::DoGet() {
        std::vector<FMSObjects::DeviceConnectionInformation> Devices;
        Poco::JSON::Object AnswerObj;
        Poco::JSON::Array AnswerArr;
        if (Storage()->GetDevices(QB_.Offset, QB_.Limit, Devices)) {
            for (const auto &i:Devices) {
                Poco::JSON::Object Obj;
                i.to_json(Obj);
                AnswerArr.add(Obj);
            }
            AnswerObj.set(RESTAPI::Protocol::DEVICES, AnswerArr);
            ReturnObject(AnswerObj);
            return;
        }
        AnswerObj.set(RESTAPI::Protocol::DEVICES, AnswerArr);
        ReturnObject(AnswerObj);
    }
}