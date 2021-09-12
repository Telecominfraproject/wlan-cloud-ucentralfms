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
        try {
            InitQueryBlock();

            std::vector<FMSObjects::DeviceConnectionInformation> Devices;
            if (Storage()->GetDevices(QB_.Offset, QB_.Limit, Devices)) {
                Poco::JSON::Array AnswerArr;
                for (const auto &i:Devices) {
                    Poco::JSON::Object Obj;
                    i.to_json(Obj);
                    AnswerArr.add(Obj);
                }
                Poco::JSON::Object AnswerObj;
                AnswerObj.set(RESTAPI::Protocol::DEVICES, AnswerArr);
                ReturnObject(AnswerObj);
                return;
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest("Internal error.");
    }
}