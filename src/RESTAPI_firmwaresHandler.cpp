//
// Created by stephane bourque on 2021-05-09.
//

#include "RESTAPI_firmwaresHandler.h"
#include "StorageService.h"
#include "LatestFirmwareCache.h"

namespace uCentral {
    void RESTAPI_firmwaresHandler::handleRequest(Poco::Net::HTTPServerRequest &Request,
                                                 Poco::Net::HTTPServerResponse &Response) {
        if (!ContinueProcessing(Request, Response))
            return;

        if (!IsAuthorized(Request, Response))
            return;

        ParseParameters(Request);
        if (Request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET)
            DoGet(Request, Response);
        else
            BadRequest(Request, Response);
    }

    void
    RESTAPI_firmwaresHandler::DoGet(Poco::Net::HTTPServerRequest &Request, Poco::Net::HTTPServerResponse &Response) {
        try {

            InitQueryBlock();
            auto DeviceType = GetParameter("deviceType","");

            // special cases: if latestOnly and deviceType
            if(HasParameter("latestOnly") && HasParameter("deviceType")) {
                std::cout << __LINE__ << std::endl;
                bool LatestOnly = GetParameter("latestOnly",false);
                std::cout << __LINE__ << std::endl;

                //  Let's find the ID of the latest
                if(LatestOnly) {
                    std::cout << __LINE__ << std::endl;
                    auto FirmwareId = LatestFirmwareCache()->FindLatestFirmware(DeviceType);
                    std::cout << __LINE__ << std::endl;
                    if(FirmwareId.empty()) {
                        std::cout << __LINE__ << std::endl;
                        NotFound(Request, Response);
                        return;
                    }

                    FMSObjects::Firmware    F;
                    std::cout << __LINE__ << std::endl;
                    if(Storage()->GetFirmware(FirmwareId,F)) {
                        std::cout << __LINE__ << std::endl;
                        Poco::JSON::Object  Answer;
                        F.to_json(Answer);
                        ReturnObject(Request, Answer, Response);
                        return;
                    }
                    std::cout << __LINE__ << std::endl;
                    NotFound(Request, Response);
                    return;
                } else {
                    std::cout << __LINE__ << std::endl;
                    std::vector<FMSObjects::Firmware> List;
                    if (Storage()->GetFirmwares(QB_.Offset, QB_.Limit, DeviceType, List)) {
                        Poco::JSON::Array ObjectArray;
                        std::cout << __LINE__ << std::endl;
                        for (const auto &i:List) {
                            Poco::JSON::Object Obj;
                            i.to_json(Obj);
                            ObjectArray.add(Obj);
                        }
                        Poco::JSON::Object RetObj;
                        RetObj.set("firmwares", ObjectArray);
                        ReturnObject(Request, RetObj, Response);
                        return;
                    } else {
                        NotFound(Request, Response);
                        return;
                    }
                }
            }

            std::vector<FMSObjects::Firmware> List;
            if (Storage()->GetFirmwares(QB_.Offset, QB_.Limit, DeviceType, List)) {

                Poco::JSON::Array ObjectArray;

                for (const auto &i:List) {
                    Poco::JSON::Object Obj;
                    i.to_json(Obj);
                    ObjectArray.add(Obj);
                }
                Poco::JSON::Object RetObj;
                RetObj.set("firmwares", ObjectArray);
                ReturnObject(Request, RetObj, Response);
                return;
            }
        } catch (const Poco::Exception &E) {
            Logger_.log(E);
        }
        BadRequest(Request, Response);
    }
}
