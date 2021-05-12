//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#ifndef UCENTRAL_UFILEUPLOADER_H
#define UCENTRAL_UFILEUPLOADER_H

#include "uSubSystemServer.h"

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerRequest.h"

namespace uCentral::uFileUploader {
    int Start();
    void Stop();
    const std::string & FullName();
    const std::string & Path();

    class Service : public uSubSystemServer {
    public:
        Service() noexcept;

        friend int Start();
        friend void Stop();
        friend const std::string & FullName();
        friend const std::string & Path();

        static Service *instance() {
            if (instance_ == nullptr) {
                instance_ = new Service;
            }
            return instance_;
        }

    private:
        int Start() override;
        void Stop() override;
        const std::string & FullName();
        const std::string & Path() { return Path_; };

        static Service *instance_;
        std::vector<std::unique_ptr<Poco::Net::HTTPServer>>   Servers_;
		Poco::ThreadPool				Pool_;
        std::string                     FullName_;
        std::string                     Path_;
    };

    class RequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
    public:
        explicit RequestHandlerFactory(Poco::Logger &L) :
                Logger_(L){}

        Poco::Net::HTTPRequestHandler *createRequestHandler(const Poco::Net::HTTPServerRequest &request) override;
    private:
        Poco::Logger    & Logger_;
    };

} //   namespace

#endif //UCENTRAL_UFILEUPLOADER_H
