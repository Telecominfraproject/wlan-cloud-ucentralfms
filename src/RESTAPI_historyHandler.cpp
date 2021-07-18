//
// Created by stephane bourque on 2021-07-13.
//

#include "RESTAPI_historyHandler.h"

//
// Created by stephane bourque on 2021-05-09.
//

#include "RESTAPI_historyHandler.h"
#include "StorageService.h"

namespace uCentral {
    void RESTAPI_historyHandler::handleRequest(Poco::Net::HTTPServerRequest &Request,
                                               Poco::Net::HTTPServerResponse &Response) {
        if (!ContinueProcessing(Request, Response))
            return;
        if (!IsAuthorized(Request, Response))
            return;
        std::cout << __LINE__ << std::endl;
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
            std::cout << __LINE__ << std::endl;
            auto SerialNumber = GetBinding("serialNumber", "");
            std::cout << __LINE__ << std::endl;

            if (!SerialNumber.empty()) {
                std::cout << __LINE__ << std::endl;
                FMSObjects::RevisionHistoryEntryVec H;
                InitQueryBlock();
                if (Storage()->GetHistory(SerialNumber, QB_.Offset, QB_.Limit, H)) {
                    std::cout << __LINE__ << std::endl;
                    Poco::JSON::Array A;
                    for (auto const &i:H) {
                        Poco::JSON::Object O;
                        i.to_json(O);
                        A.add(O);
                    }
                    Poco::JSON::Object Answer;
                    Answer.set("history", A);
                    ReturnObject(Request, Answer, Response);
                } else {
                    NotFound(Request, Response);
                }
                return;
            }
            std::cout << __LINE__ << std::endl;
        } catch (const Poco::Exception &E) {
            std::cout << __LINE__ << std::endl;
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }

    void RESTAPI_historyHandler::DoDelete(Poco::Net::HTTPServerRequest &Request,
                                          Poco::Net::HTTPServerResponse &Response) {
        try {
            auto SerialNumber = GetBinding("serialNumber", "");
            auto Id = GetParameter("id", "");
            if (SerialNumber.empty() || Id.empty()) {
                BadRequest(Request, Response, "SerialNumber and Id must not be empty.");
                return;
            }

            if (!Storage()->DeleteHistory(SerialNumber, Id)) {
                OK(Request, Response);
                return;
            }
            NotFound(Request, Response);
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }
}