//
// Created by stephane bourque on 2021-07-13.
//

#include "RESTAPI_historyHandler.h"
#include "StorageService.h"
#include "framework/RESTAPI_protocol.h"
#include "framework/RESTAPI_errors.h"

namespace OpenWifi {
    void
    RESTAPI_historyHandler::DoGet() {
        auto SerialNumber = GetBinding(RESTAPI::Protocol::SERIALNUMBER, "");

        if(SerialNumber.empty()) {
            return BadRequest(RESTAPI::Errors::MissingSerialNumber);
        }

        FMSObjects::RevisionHistoryEntryVec H;
        if (StorageService()->GetHistory(SerialNumber, QB_.Offset, QB_.Limit, H)) {
            Poco::JSON::Array A;
            for (auto const &i:H) {
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

        if (!StorageService()->DeleteHistory(SerialNumber, Id)) {
            return OK();
        }
        NotFound();
    }
}