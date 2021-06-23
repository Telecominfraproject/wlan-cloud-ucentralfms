//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#ifndef UCENTRAL_UAUTHSERVICE_H
#define UCENTRAL_UAUTHSERVICE_H

#include <map>

#include "SubSystemServer.h"

#include "Poco/JSON/Object.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/JWT/Signer.h"
#include "Poco/SHA2Engine.h"
#include "Poco/MD5Engine.h"
#include "Poco/DigestEngine.h"

#include "RESTAPI_objects.h"

namespace uCentral {

	enum ACCESS_TYPE {
		USERNAME,
		SERVER,
		CUSTOM
	};

	ACCESS_TYPE IntToAccessType(int C);
	int AccessTypeToInt(ACCESS_TYPE T);

	enum API_KEY_ACCESS {
	    NONE,
	    ALL,
	    CALLBACK,
	    NEWFIRMWARENOTIFIER
	};

	struct APIKeyEntry {
	    std::string     Key;
        API_KEY_ACCESS  Access;
	    std::string     Owner;
	    std::string     Description;
	};

    class AuthService : public SubSystemServer {
    public:

        AuthService() noexcept;

        static AuthService *instance() {
            if (instance_ == nullptr) {
                instance_ = new AuthService;
            }
            return instance_;
        }

        int Start() override;
        void Stop() override;
        bool IsAuthorized(Poco::Net::HTTPServerRequest & Request,std::string &SessionToken, struct uCentral::Objects::WebToken & UserInfo );
        void CreateToken(const std::string & UserName, uCentral::Objects::WebToken & ResultToken, uCentral::Objects::AclTemplate & ACL);
        bool Authorize( const std::string & UserName, const std::string & Password, uCentral::Objects::WebToken & ResultToken );
        void Logout(const std::string &token);
        bool IsValidAPIKey(const std::string &APIKey, APIKeyEntry & Entry);
        bool IsValidAPIKey(Poco::Net::HTTPServerRequest & Request, APIKeyEntry & Entry);
        void InitAPIKeys();
        APIKeyEntry GetFirst() { return APIKeys_.begin()->second; }
        std::string GenerateToken(const std::string & Identity, ACCESS_TYPE Type, int NumberOfDays);
        bool ValidateToken(const std::string & Token, std::string & SessionToken, struct uCentral::Objects::WebToken & UserInfo);
        [[nodiscard]] std::string ComputePasswordHash(const std::string &UserName, const std::string &Password);

    private:
		static AuthService *instance_;
		std::map<std::string,uCentral::Objects::WebToken>   Tokens_;
		bool    			        Secure_ = false ;
		std::string     	        DefaultUserName_;
		std::string			        DefaultPassword_;
		std::string     	        Mechanism_;
		bool            	        AutoProvisioning_ = false ;
		Poco::JWT::Signer	                Signer_;
		Poco::SHA2Engine	                SHA2_;
		std::string                         ApiKeyDb_;
        std::map<std::string,APIKeyEntry>   APIKeys_;
    };

    inline AuthService * AuthService() { return AuthService::instance(); };
}; // end of namespace

#endif //UCENTRAL_UAUTHSERVICE_H
