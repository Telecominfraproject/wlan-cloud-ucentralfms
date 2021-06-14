//
// Created by stephane bourque on 2021-05-11.
//

#ifndef UCENTRALFWS_RESTAPI_CALLBACKCHANNEL_H
#define UCENTRALFWS_RESTAPI_CALLBACKCHANNEL_H

#include "RESTAPI_handler.h"

namespace uCentral {
    class RESTAPI_callbackChannel : public RESTAPIHandler {
    public:
        RESTAPI_callbackChannel(const RESTAPIHandler::BindingMap &bindings, Poco::Logger &L)
                : RESTAPIHandler(bindings, L,
                                 std::vector<std::string>
                                         {Poco::Net::HTTPRequest::HTTP_POST,
                                          Poco::Net::HTTPRequest::HTTP_OPTIONS}) {}

        void handleRequest(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response) override;

        void DoPost(Poco::Net::HTTPServerRequest &request, Poco::Net::HTTPServerResponse &response);
    };
}

#endif //UCENTRALFWS_RESTAPI_CALLBACKCHANNEL_H
