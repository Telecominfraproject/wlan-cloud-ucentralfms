//
// Created by stephane bourque on 2021-05-11.
//

#include "RESTAPI_callbackChannel.h"
#include "StorageService.h"
#include "Daemon.h"

#include "Poco/JSON/Parser.h"

void RESTAPI_callbackChannel::handleRequest(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) {

    ParseParameters(Request);
    if (Request.getMethod()==Poco::Net::HTTPRequest::HTTP_POST)
        DoPost(Request, Response);
    else
        BadRequest(Response);
}

void RESTAPI_callbackChannel::DoPost(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) {
    try {
        uCentral::Auth::APIKeyEntry Entry;

        if(!IsValidAPIKey(Request, Response, Entry)) {
            UnAuthorized(Response);
            return;
        }

        auto Subscribe = GetParameter("subscribe","");
        if(Subscribe=="true") {
            //  subscribing
            //  we must have uri, msgs, id, key
            auto URI = GetParameter("uri","");
            auto Topics = GetParameter("topics","");
            auto ID = GetParameter("id","");
            auto Key = GetParameter("key","");

            if(URI.empty() || Topics.empty() || ID.empty() || Key.empty()) {
                BadRequest(Response);
                return;
            }

            uCentral::Objects::Callback C;

            C.UUID = ID;
            C.TokenType = "X-API-KEY";
            C.Token = Key;
            C.URI = URI;
            C.Created = time(nullptr);
            C.Creator = Entry.Owner;
            C.Topics = Topics;

            if(uCentral::Storage::AddOrUpdateCallback(C)) {
                Logger_.information(Poco::format("CALLBACK(%s): Just subscribed.",Entry.Owner));
                OK(Response);
                return;
            } else {
                Logger_.error(Poco::format("CALLBACK(%s): Could not register.",Entry.Owner));
                BadRequest(Response);
                return;
            }
        } else if (Subscribe=="false") {
            // removing a subscription
            auto ID = GetParameter("id","");
            if(ID.empty()) {
                BadRequest(Response);
                return;
            }
            if(uCentral::Storage::DeleteCallback(ID)) {
                OK(Response);
                return;
            }
            Logger_.error(Poco::format("CALLBACK(%s): Cannot remove subscription.",Entry.Owner));
            NotFound(Response);
            return;
        } else {
            // not a subscription message
            auto Topic = GetParameter("topic","");
            if(Topic.empty()) {
                BadRequest(Response);
                return;
            }
            Poco::JSON::Parser parser;
            Poco::JSON::Object::Ptr Obj = parser.parse(Request.stream()).extract<Poco::JSON::Object::Ptr>();
            OK(Response);
            return;
        }
    } catch (const Poco::Exception &E) {
        Logger_.log(E);
    }
    BadRequest(Response);
}

