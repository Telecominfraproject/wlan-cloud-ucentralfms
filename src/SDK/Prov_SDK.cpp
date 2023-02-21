//
// Created by stephane bourque on 2021-10-04.
//

#include "framework/MicroServiceNames.h"
#include "framework/OpenAPIRequests.h"

namespace OpenWifi::SDK::Prov {
	bool GetFirmwareOptions(const std::string &serialNumber, std::string &firmwareUpgrade,
							bool &firmwareRCOnly) {

		Types::StringPairVec QueryData;
		QueryData.push_back(std::make_pair("firmwareOptions", "true"));

		OpenWifi::OpenAPIRequestGet R(OpenWifi::uSERVICE_PROVISIONING,
									  "/api/v1/inventory/" + serialNumber, QueryData, 10000);
		firmwareUpgrade = "no";
		firmwareRCOnly = false;
		Poco::JSON::Object::Ptr Response;
		if (R.Do(Response) == Poco::Net::HTTPResponse::HTTP_OK) {
			std::cout << "Received options... " << std::endl;
			std::ostringstream os;
			Poco::JSON::Stringifier::stringify(Response, os);
			std::cout << "Firmware option response - good - Response: " << os.str() << std::endl;

			if (Response->has("firmwareUpgrade"))
				firmwareUpgrade = Response->get("firmwareUpgrade").toString();
			if (Response->has("firmwareRCOnly"))
				firmwareRCOnly = Response->get("firmwareRCOnly").toString() == "true";
			return true;
		} else {
			std::cout << "Failed Received options... " << std::endl;
			std::ostringstream os;
			Poco::JSON::Stringifier::stringify(Response, os);
			std::cout << "Firmware option response - bad- Response: " << os.str() << std::endl;
		}

		return false;
	}

} // namespace OpenWifi::SDK::Prov