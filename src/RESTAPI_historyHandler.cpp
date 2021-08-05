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

namespace uCentral {
    void RESTAPI_historyHandler::handleRequest(Poco::Net::HTTPServerRequest &Request,
                                               Poco::Net::HTTPServerResponse &Response) {
        if (!ContinueProcessing(Request, Response))
            return;
        if (!IsAuthorized(Request, Response))
            return;
        ParseParameters(Request);
        if (Request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            DoGet(Request, Response);
        else if(Request.getMethod() == Poco::Net::HTTPRequest::HTTP_DELETE)
            DoDelete(Request, Response);
        else
            BadRequest(Request, Response);
    }

    void
    RESTAPI_historyHandler::DoGet(Poco::Net::HTTPServerRequest &Request, Poco::Net::HTTPServerResponse &Response) {
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
                    ReturnObject(Request, Answer, Response);
                } else {
                    NotFound(Request, Response);
                }
                return;
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }

    void RESTAPI_historyHandler::DoDelete(Poco::Net::HTTPServerRequest &Request,
                                          Poco::Net::HTTPServerResponse &Response) {
        try {
            auto SerialNumber = GetBinding(RESTAPI::Protocol::SERIALNUMBER, "");
            auto Id = GetParameter(RESTAPI::Protocol::ID, "");
            if (SerialNumber.empty() || Id.empty()) {
                BadRequest(Request, Response, "SerialNumber and Id must not be empty.");
                return;
            }

            if (!Storage()->DeleteHistory(SerialNumber, Id)) {
                OK(Request, Response);
                return;
            }
            NotFound(Request, Response);
            return;
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }
}