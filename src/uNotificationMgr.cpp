//
// Created by stephane bourque on 2021-05-11.
//

#include "uNotificationMgr.h"
#include "uStorageService.h"
#include "RESTAPI_objects.h"

#include "Poco/JSON/Object.h"
#include "Poco/URI.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/MediaType.h"

namespace uCentral::NotificationMgr {

    Service *Service::instance_ = nullptr;

    int Start() {
        return Service::instance()->Start();
    }

    void Stop() {
        Service::instance()->Stop();
    }

    void Update() {
        Service::instance()->Update();
    }

    Service::Service() noexcept:
            uSubSystemServer("NotificationMgr", "NOTIFY-MGR", "nodifymgr") {
    }

    int Service::Start() {
        SubMutexGuard Guard(Mutex_);

        Logger_.information("Starting ");
        Worker_.start(*this);

        return 0;
    }

    void Service::Stop() {
        SubMutexGuard Guard(Mutex_);

        Logger_.information("Stopping ");
        Running_ = false;

        Worker_.wakeUp();
        Worker_.join();
    }

    void Service::run() {

        Running_ = true;

        while (Running_) {
            Poco::Thread::trySleep(2000000000);

            if(!Running_)
                break;
            if(!Updated_)
                continue;
            Updated_ = false;

            if(uCentral::Storage::FirmwareVersion()!=ManifestVersion_) {

                Poco::JSON::Object Manifest;
                uCentral::Storage::BuildFirmwareManifest(Manifest, ManifestVersion_);

                std::stringstream OS;
                Poco::JSON::Stringifier stringifier;

                stringifier.condense(Manifest, OS);

                std::cout << "Manifest" << std::endl;
                std::cout << OS.str() << std::endl;
                CurrentManifest_ = OS.str();
            }

            //  Send it...
            NotifyCallers();
        }
    }

    void Service::Update() {
        SubMutexGuard Guard(Mutex_);

        Updated_ = true;
        Worker_.wakeUp();
    }

    void Service::NotifyCallers() {

        std::vector<uCentral::Objects::Callback> Callbacks;

        //  build the list of callbacks or update the existing callers.
        if(uCentral::Storage::GetCallbacks(0,200,Callbacks)) {
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

        std::cout << __LINE__ << std::endl;

        if(EndPoints_.empty())
            return;

        std::cout << __LINE__ << std::endl;
        for(auto & host:EndPoints_) {
            std::cout << __LINE__ << std::endl;
            if(host.second.LastVersion!=ManifestVersion_) {
                std::cout << __LINE__ << std::endl;
                if(SendManifest(host.second.Caller)) {
                    std::cout << __LINE__ << std::endl;
                    host.second.LastVersion = ManifestVersion_;
                    host.second.LasContact = time(nullptr);
                }
            }
        }
    }

    bool DoRequest(Poco::Net::HTTPSClientSession& Session, Poco::Net::HTTPRequest& Request, Poco::Net::HTTPResponse& Response, const std::string & Doc)
    {
        std::cout << __LINE__ << std::endl;
        std::stringstream Body(Doc);
        Request.setContentType("application/json");
        std::cout << __LINE__ << std::endl;
        Request.setContentLength(Doc.length());
        std::cout << __LINE__ << std::endl;
        std::ostream& OS = Session.sendRequest(Request);
        std::cout << __LINE__ << std::endl;
        // Poco::StreamCopier::copyStream(Body, OS);
        std::cout << __LINE__ << " Wrote bytes:" << Poco::StreamCopier::copyStream(Body, OS) << std::endl;

        Session.receiveResponse(Response);
        std::cout << __LINE__ << std::endl;
        return (Response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK);
    }

    bool Service::SendManifest(const uCentral::Objects::Callback &Host) {

        Poco::URI Uri(Host.URI);

        Uri.addQueryParameter("topic", "ucentralfws");

        Poco::Net::HTTPSClientSession Session(Uri.getHost(), Uri.getPort());
        Poco::Net::HTTPRequest Request(Poco::Net::HTTPRequest::HTTP_POST,
                                       Uri.getPathAndQuery(),
                                       Poco::Net::HTTPMessage::HTTP_1_1);
        Request.add("X-API-KEY", Host.Token);
        std::cout << __LINE__ << Uri.toString() << std::endl;

        Poco::Net::HTTPResponse Response;
        return DoRequest(Session, Request, Response, CurrentManifest_);
    }

}
