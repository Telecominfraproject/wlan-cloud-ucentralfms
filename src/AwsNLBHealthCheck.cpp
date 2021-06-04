//
// Created by stephane bourque on 2021-06-04.
//

#include "AwsNLBHealthCheck.h"

#include "uFirmwareDS.h"
#include "uUtils.h"

void AwsNLBHealthCheck::run() {

	static std::string NLBMessage{"uCentralGW is here.\r\n"};
	static std::string NLBResponse{"HTTP/1.1 200 OK\r\nServer: NLB Healthcheck\r\nConnection: close\r\nContent-Length : " + std::to_string(NLBMessage.length()) +"\r\n\r\n"+NLBMessage};

	Running_ = true;
	Sock_.bind(uCentral::ServiceConfig::GetInt("nlb.port",15016));
	Sock_.listen();

	while(Running_) {
		try {
		    DBGLINE
			auto NewSock = Sock_.acceptConnection();
            DBGLINE
			if (Running_) {
                DBGLINE
				auto b = NewSock.sendBytes(NLBResponse.c_str(), (int)NLBResponse.size());
				NewSock.shutdown();
				NewSock.close();
                DBGLINE
			}
		} catch(const Poco::Exception &E) {
            DBGLINE
			break;
		}
	}
    DBGLINE
}

int AwsNLBHealthCheck::Start() {
	if(uCentral::ServiceConfig::GetBool("nlb.enable",false)) {
		Th_.start(*this);
	}
	return 0;
}

void AwsNLBHealthCheck::Stop() {
	if(Running_) {
        DBGLINE
		Running_ = false;
        DBGLINE
		Sock_.impl()->close();
        DBGLINE
		Th_.join();
        DBGLINE
	}
}
