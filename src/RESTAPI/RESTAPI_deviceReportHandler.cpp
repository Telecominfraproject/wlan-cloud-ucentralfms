//
// Created by stephane bourque on 2021-07-19.
//

#include "RESTAPI_deviceReportHandler.h"
#include "RESTObjects/RESTAPI_FMSObjects.h"
#include "Poco/JSON/Object.h"
#include "Daemon.h"

namespace OpenWifi {
    void RESTAPI_deviceReportHandler::DoGet() {
        poco_information(Logger(),fmt::format("GET-DASHBOARD: {}", Requester()));
        FMSObjects::DeviceReport	Data;
        if(Daemon()->GetDashboard().Get(Data, Logger())) {
            Poco::JSON::Object Answer;
            Data.to_json(Answer);
            return ReturnObject(Answer);
        }
        return BadRequest(RESTAPI::Errors::InternalError);
    }
}