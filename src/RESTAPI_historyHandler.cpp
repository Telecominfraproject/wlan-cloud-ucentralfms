//
// Created by stephane bourque on 2021-07-13.
//

#include "RESTAPI_historyHandler.h"

//
// Created by stephane bourque on 2021-05-09.
//

#include "RESTAPI_historyHandler.h"
#include "StorageService.h"
#include "RESTAPI_protocol.h"

namespace OpenWifi {
    void
    RESTAPI_historyHandler::DoGet() {
        try {
            auto SerialNumber = GetBinding(RESTAPI::Protocol::SERIALNUMBER, "");

            if (!SerialNumber.empty()) {
                FMSObjects::RevisionHistoryEntryVec H;
                InitQueryBlock();
                if (Storage()->GetHistory(SerialNumber, QB_.Offset, QB_.Limit, H)) {
                    Poco::JSON::Array A;
                    for (auto const &i:H) {
                        Poco::JSON::Object O;
                        i.to_json(O);
                        A.add(O);
                    }
                    Poco::JSON::Object Answer;
                    Answer.set(RESTAPI::Protocol::HISTORY, A);
                    ReturnObject(Answer);
                } else {
                    NotFound();
                }
                return;
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest("Internal error.");
    }

    void RESTAPI_historyHandler::DoDelete() {
        try {
            auto SerialNumber = GetBinding(RESTAPI::Protocol::SERIALNUMBER, "");
            auto Id = GetParameter(RESTAPI::Protocol::ID, "");
            if (SerialNumber.empty() || Id.empty()) {
                BadRequest("SerialNumber and Id must not be empty.");
                return;
            }

            if (!Storage()->DeleteHistory(SerialNumber, Id)) {
                OK();
                return;
            }
            NotFound();
            return;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest("Internal error.");
    }
}