//
// Created by stephane bourque on 2021-10-05.
//

#include "GW_SDK.h"
#include "Daemon.h"
#include "Poco/Net/HTTPResponse.h"

namespace OpenWifi::SDK::GW {

    bool SendFirmwareUpgradeCommand( const std::string & serialNumber, const std::string & URI, [[maybe_unused]] uint64_t When  ) {
        Types::StringPairVec    QueryData;
        Poco::JSON::Object      Body;

        Body.set("serialNumber", serialNumber);
        Body.set("uri", URI);
        Body.set("when",0);

        OpenWifi::OpenAPIRequestPost R(OpenWifi::uSERVICE_GATEWAY,
                                       "/api/v1/device/" + serialNumber + "/upgrade" ,
                                      QueryData,
                                      Body,
                                      10000);
        Poco::JSON::Object::Ptr Response;
        if(R.Do(Response) == Poco::Net::HTTPResponse::HTTP_OK) {
            std::ostringstream os;
            Poco::JSON::Stringifier::stringify(Response,os);
            std::cout << "FirmwareUpgradeCommand - good - response: " << os.str() << std::endl;
            return true;
        } else {
            std::ostringstream os;
            Poco::JSON::Stringifier::stringify(Response,os);
            std::cout << "FirmwareUpgradeCommand - bad - response: " << os.str() << std::endl;
        }

        return false;
    }

}