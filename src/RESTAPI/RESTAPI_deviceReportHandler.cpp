//
// Created by stephane bourque on 2021-07-19.
//

#include "RESTAPI_deviceReportHandler.h"
#include "RESTObjects/RESTAPI_FMSObjects.h"
#include "Poco/JSON/Object.h"
#include "Daemon.h"

namespace OpenWifi {
    void RESTAPI_deviceReportHandler::DoGet() {
        Daemon()->CreateDashboard();
        Poco::JSON::Object  O;
        Daemon()->GetDashboard().to_json(O);
        ReturnObject(O);
    }
}