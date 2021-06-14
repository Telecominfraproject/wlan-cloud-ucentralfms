//
// Created by stephane bourque on 2021-05-11.
//
#include <iostream>
#include <fstream>

#include "NotificationMgr.h"
#include "StorageService.h"
#include "RESTAPI_objects.h"

#include "Poco/JSON/Object.h"
#include "Poco/URI.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/MediaType.h"

namespace uCentral {

    class NotificationMgr *NotificationMgr::instance_ = nullptr;

    NotificationMgr::NotificationMgr() noexcept:
            SubSystemServer("NotificationMgr", "NOTIFY-MGR", "nodifymgr") {
    }

    int NotificationMgr::Start() {
        SubMutexGuard Guard(Mutex_);

        Logger_.information("Starting ");
        Worker_.start(*this);

        return 0;
    }

    void NotificationMgr::Stop() {
        SubMutexGuard Guard(Mutex_);

        Logger_.information("Stopping ");
        Running_ = false;

        Worker_.wakeUp();
        Worker_.join();
    }

    void NotificationMgr::run() {

        Running_ = true;

        while (Running_) {
            Poco::Thread::trySleep(2000000000);

            if(!Running_)
                break;
            if(!Updated_)
                continue;
            Updated_ = false;

            if(uCentral::Storage()->FirmwareVersion()!=ManifestVersion_) {
                Poco::JSON::Object Manifest;

                uCentral::Storage()->BuildFirmwareManifest(Manifest, ManifestVersion_);
                std::stringstream OS;
                Poco::JSON::Stringifier stringifier;
                stringifier.condense(Manifest, OS);
                CurrentManifest_ = OS.str();
            }

            //  Send it...
            NotifyCallers();
        }
    }

    void NotificationMgr::Update() {
        SubMutexGuard Guard(Mutex_);

        Updated_ = true;
        Worker_.wakeUp();
    }

    void NotificationMgr::NotifyCallers() {

        std::vector<uCentral::Objects::Callback> Callbacks;

        std::ofstream File( "latest_manifest.json" , std::ofstream::out | std::ofstream::trunc);
        File << CurrentManifest_;
        File.close();

        //  build the list of callbacks or update the existing callers.
        if(uCentral::Storage()->GetCallbacks(0,200,Callbacks)) {
            for(const auto & i:Callbacks) {
                auto Index = EndPoints_.find(i.UUID);
                if(Index==EndPoints_.end()) {
                    NotifyEndPoint  E{
                        .Caller = i,
                        .LasContact = 0,
                        .LastVersion = 0};
                    EndPoints_[i.UUID] = E;
                } else {
                    Index->second.Caller = i;
                }
            }
        }

        if(EndPoints_.empty())
            return;

        for(auto & host:EndPoints_) {
            if(host.second.LastVersion!=ManifestVersion_) {
                if(SendManifest(host.second.Caller)) {
                    host.second.LastVersion = ManifestVersion_;
                    host.second.LasContact = time(nullptr);
                }
            }
        }
    }

    bool DoRequest(Poco::Net::HTTPSClientSession& Session, Poco::Net::HTTPRequest& Request, Poco::Net::HTTPResponse& Response, const std::string & Doc)
    {
        std::stringstream Body(Doc);
        Request.setContentType("application/json");
        Request.setContentLength(Doc.length());
        std::ostream& OS = Session.sendRequest(Request);
        Poco::StreamCopier::copyStream(Body, OS);
        Session.receiveResponse(Response);
        return (Response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK);
    }

    bool NotificationMgr::SendManifest(const uCentral::Objects::Callback &Host) {

        Poco::URI Uri(Host.URI);

        Uri.addQueryParameter("topic", "ucentralfws");

        Poco::Net::HTTPSClientSession Session(Uri.getHost(), Uri.getPort());
        Poco::Net::HTTPRequest Request(Poco::Net::HTTPRequest::HTTP_POST,
                                       Uri.getPathAndQuery(),
                                       Poco::Net::HTTPMessage::HTTP_1_1);
        Request.add("X-API-KEY", Host.Token);
        Poco::Net::HTTPResponse Response;
        return DoRequest(Session, Request, Response, CurrentManifest_);
    }

}
