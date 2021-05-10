//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#include <iostream>
#include <fstream>
#include <cstdio>

#include "uFileUploader.h"
#include "FirmwareDS.h"
#include "uStorageService.h"

#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/DynamicAny.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/PartHandler.h"
#include "Poco/Net/MessageHeader.h"
#include "Poco/CountingStream.h"
#include "Poco/StreamCopier.h"
#include "Poco/Exception.h"
#include "poco/File.h"

#include "utils.h"

namespace uCentral::uFileUploader {
    Service *Service::instance_ = nullptr;

    int Start() {
        return uCentral::uFileUploader::Service::instance()->Start();
    }

    void Stop() {
        uCentral::uFileUploader::Service::instance()->Stop();
    }

    const std::string & FullName() {
        return uCentral::uFileUploader::Service::instance()->FullName();
    }

    const std::string & Path() {
        return uCentral::uFileUploader::Service::instance()->Path();
    }

    Service::Service() noexcept:
            SubSystemServer("FileUploader", "FILE-UPLOAD", "ucentralfws.fileuploader")
    {
		SubMutexGuard		Guard(Mutex_);
    }

    static const std::string URI_BASE{"/v1/upload/"};

    int Service::Start() {
        Logger_.notice("Starting.");

        for(const auto & Svr: ConfigServersList_) {
            std::string l{"Starting: " +
                          Svr.Address() + ":" + std::to_string(Svr.Port()) +
                          " key:" + Svr.KeyFile() +
                          " cert:" + Svr.CertFile()};

            Logger_.information(l);

            Path_ = uCentral::ServiceConfig::GetString("ucentralfws.fileuploader.path","/tmp");

            auto Sock{Svr.CreateSecureSocket(Logger_)};

			Svr.LogCert(Logger_);
			if(!Svr.RootCA().empty())
				Svr.LogCas(Logger_);

            auto Params = new Poco::Net::HTTPServerParams;
            Params->setMaxThreads(16);
            Params->setMaxQueued(100);

            if(FullName_.empty()) {
                FullName_ = "https://" + Svr.Name() + ":" + std::to_string(Svr.Port()) + URI_BASE;
                Logger_.information(Poco::format("Uploader URI base is '%s'", FullName_));
            }
            auto NewServer = std::make_unique<Poco::Net::HTTPServer>(new RequestHandlerFactory(Logger_), Pool_, Sock, Params);
            NewServer->start();
            Servers_.push_back(std::move(NewServer));
        }

        return 0;
    }

    const std::string & Service::FullName() {
        return FullName_;
    }

    class MyPartHandler: public Poco::Net::PartHandler
    {
    public:
        MyPartHandler(std::string UUID, Poco::Logger & Logger):
            UUID_(std::move(UUID)),
            Logger_(Logger)
        {
            Dir_ = uCentral::uFileUploader::Path() + "/" + UUID_;
            Poco::File  D(Dir_);
        }

        void Init() {
            Poco::File  D(Dir_);
            D.createDirectory();
        }

        [[nodiscard]] const std::vector<std::string> & Files() const { return Files_; }

        void handlePart(const Poco::Net::MessageHeader& Header, std::istream& Stream) override
        {
            if (Header.has("Content-Disposition"))
            {
                std::string Disposition;
                Poco::Net::NameValueCollection Parameters;
                Poco::Net::MessageHeader::splitParameters(Header["Content-Disposition"], Disposition, Parameters);
                Name_ = Parameters.get("filename", "(unnamed)");
            }

            Poco::CountingInputStream InputStream(Stream);
            std::string FinalFileName = Dir_ + "/" + Name_ ;
            std::string TmpFileName = FinalFileName + ".upload.start" ;
            Logger_.information(Poco::format("FILE-UPLOADER: uploading %s",FinalFileName));

            std::ofstream OutputStream(TmpFileName, std::ofstream::out);
            Poco::StreamCopier::copyStream(InputStream, OutputStream);
            rename(TmpFileName.c_str(),FinalFileName.c_str());
            Files_.push_back(Name_);
        }

    private:
        std::string                 Name_;
        std::string                 Dir_;
        std::string                 UUID_;
        Poco::Logger               &Logger_;
        std::vector<std::string>    Files_;
    };


    class FormRequestHandler: public Poco::Net::HTTPRequestHandler
        /// Return a HTML document with the current date and time.
    {
    public:
        explicit FormRequestHandler(std::string UUID, Poco::Logger & L):
            UUID_(std::move(UUID)),
            Logger_(L)
        {
        }

        void handleRequest(Poco::Net::HTTPServerRequest& Request, Poco::Net::HTTPServerResponse& Response) override
        {
            try {
                MyPartHandler partHandler(UUID_,Logger_);

                partHandler.Init();
                Poco::Net::HTMLForm form(Request, Request.stream(), partHandler);

                Response.setChunkedTransferEncoding(true);
                Response.setContentType("text/html");
                std::ostream &ResponseStream = Response.send();

                ResponseStream <<
                     "<html>\n"
                     "<head>\n"
                     "<title>uCentralFWS Uploader</title>\n"
                     "</head>\n"
                     "<body>\n"
                     "<h1>uCentralFWS Uploader service</h1>\n";

                 const std::vector<std::string> & FileNames = partHandler.Files();

                if (!FileNames.empty()) {
                    ResponseStream << "<p>Uploaded files</p>" ;
                    for(const auto &i:FileNames) {
                        ResponseStream << "<h2>Upload</h2><p>\n";
                        ResponseStream << "Name: " << i << "<br>\n";
                    }
                }
                ResponseStream << "</body>\n";
            }
            catch( const Poco::Exception & E )
            {
                Logger_.warning(Poco::format("Error occurred while performing upload. Error='%s'",E.displayText()));
            }
            catch( ... )
            {
            }
        }
    private:
        std::string     UUID_;
        Poco::Logger    & Logger_;
    };

    Poco::Net::HTTPRequestHandler *RequestHandlerFactory::createRequestHandler(const Poco::Net::HTTPServerRequest & Request) {
        auto UUID = uCentral::instance()->CreateUUID();
		Logger_.debug(Poco::format("REQUEST(%s): %s>%s %s", UUID,uCentral::Utils::FormatIPv6(Request.clientAddress().toString()), Request.getMethod(), Request.getURI()));

		auto Key = Request.get("X-API-KEY","");

		if(Key.empty()) {
            Logger_.error(Poco::format("REQUEST(%S): No key supplied.",UUID));
            return nullptr;
        }

		if(uCentral::Auth::IsValidAPIKey(Key)) {
            return new FormRequestHandler(UUID, Logger_);
        } else {
		    Logger_.error(Poco::format("REQUEST(%s): Invalid key",UUID));
            return nullptr;
        }
    }

    void Service::Stop() {
        Logger_.notice("Stopping ");
        for( const auto & svr : Servers_ )
            svr->stop();
    }

}  //  Namespace