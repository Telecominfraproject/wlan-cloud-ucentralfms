//
// Created by stephane bourque on 2021-07-19.
//

#include "RESTAPI_deviceReportHandler.h"
#include "StorageService.h"
#include "RESTAPI_FMSObjects.h"
#include "Poco/JSON/Object.h"
#include "Daemon.h"

namespace OpenWifi {
    void RESTAPI_deviceReportHandler::DoGet() {
        try {
            Daemon()->CreateDashboard();
            Poco::JSON::Object  O;
            Daemon()->GetDashboard().to_json(O);
            ReturnObject(O);
            return;
        } catch ( const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest("Internal error.");
    }
}