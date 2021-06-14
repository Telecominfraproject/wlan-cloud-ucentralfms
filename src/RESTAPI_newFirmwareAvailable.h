//
// Created by stephane bourque on 2021-06-02.
//

#ifndef UCENTRALFWS_RESTAPI_NEWFIRMWAREAVAILABLE_H
#define UCENTRALFWS_RESTAPI_NEWFIRMWAREAVAILABLE_H

#include "RESTAPI_handler.h"

namespace uCentral {
    class RESTAPI_newFirmwareAvailable : public RESTAPIHandler {
    public:
        RESTAPI_newFirmwareAvailable(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_GET,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS}) {}

        void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;

        void DoGet(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
    };
}

#endif //UCENTRALFWS_RESTAPI_NEWFIRMWAREAVAILABLE_H
