//
//	License type: BSD 3-Clause License
//	License copy: https://github.com/Telecominfraproject/wlan-cloud-ucentralgw/blob/master/LICENSE
//
//	Created by Stephane Bourque on 2021-03-04.
//	Arilia Wireless Inc.
//

#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Stringifier.h"

#include "RESTAPI_SecurityObjects.h"
#include "RESTAPI_utils.h"

using uCentral::RESTAPI_utils::field_to_json;
using uCentral::RESTAPI_utils::field_from_json;

namespace uCentral::SecurityObjects {

	void AclTemplate::to_json(Poco::JSON::Object &Obj) const {
		field_to_json(Obj,"Read",Read_);
		field_to_json(Obj,"ReadWrite",ReadWrite_);
		field_to_json(Obj,"ReadWriteCreate",ReadWriteCreate_);
		field_to_json(Obj,"Delete",Delete_);
		field_to_json(Obj,"PortalLogin",PortalLogin_);
	}

	bool AclTemplate::from_json(const Poco::JSON::Object::Ptr &Obj)  {
		try {
			field_from_json(Obj, "Read", Read_);
			field_from_json(Obj, "ReadWrite", ReadWrite_);
			field_from_json(Obj, "ReadWriteCreate", ReadWriteCreate_);
			field_from_json(Obj, "Delete", Delete_);
			field_from_json(Obj, "PortalLogin", PortalLogin_);
			return true;
		} catch(...) {
		}
		return false;
	}

	void WebToken::to_json(Poco::JSON::Object & Obj) const {
		Poco::JSON::Object  AclTemplateObj;
		acl_template_.to_json(AclTemplateObj);
		field_to_json(Obj,"access_token",access_token_);
		field_to_json(Obj,"refresh_token",refresh_token_);
		field_to_json(Obj,"token_type",token_type_);
		field_to_json(Obj,"expires_in",expires_in_);
		field_to_json(Obj,"idle_timeout",idle_timeout_);
		field_to_json(Obj,"created",created_);
		field_to_json(Obj,"username",username_);
		Obj.set("aclTemplate",AclTemplateObj);
	}

	bool WebToken::from_json(const Poco::JSON::Object::Ptr &Obj) {
		try {
			if (Obj->isObject("aclTemplate")) {
				Poco::JSON::Object::Ptr AclTemplate = Obj->getObject("aclTemplate");
				acl_template_.from_json(AclTemplate);
			}
			field_from_json(Obj, "access_token", access_token_);
			field_from_json(Obj, "refresh_token", refresh_token_);
			field_from_json(Obj, "token_type", token_type_);
			field_from_json(Obj, "expires_in", expires_in_);
			field_from_json(Obj, "idle_timeout", idle_timeout_);
			field_from_json(Obj, "created", created_);
			field_from_json(Obj, "username", username_);
			return true;
		} catch (...) {

		}
		return false;
	}

    void UserInfo::to_json(Poco::JSON::Object &Obj) const {
		field_to_json(Obj,"Id",Id);
		field_to_json(Obj,"name",name);
		field_to_json(Obj,"description", description);
		field_to_json(Obj,"avatar", avatar);
		field_to_json(Obj,"email", email);
		field_to_json(Obj,"validated", validated);
		field_to_json(Obj,"validationEmail", validationEmail);
		field_to_json(Obj,"validationDate", validationDate);
		field_to_json(Obj,"creationDate", creationDate);
		field_to_json(Obj,"validationURI", validationURI);
		field_to_json(Obj,"changePassword", changePassword);
		field_to_json(Obj,"lastLogin", lastLogin);
		field_to_json(Obj,"currentLoginURI", currentLoginURI);
		field_to_json(Obj,"lastPasswordChange", lastPasswordChange);
		field_to_json(Obj,"lastEmailCheck", lastEmailCheck);
		field_to_json(Obj,"waitingForEmailCheck", waitingForEmailCheck);
		field_to_json(Obj,"locale", locale);
		field_to_json(Obj,"notes", notes);
		field_to_json(Obj,"location", location);
		field_to_json(Obj,"owner", owner);
		field_to_json(Obj,"suspended", suspended);
		field_to_json(Obj,"blackListed", blackListed);
		field_to_json(Obj,"userRole", userRole);
		field_to_json(Obj,"userTypeProprietaryInfo", userTypeProprietaryInfo);
		field_to_json(Obj,"securityPolicy", securityPolicy);
		field_to_json(Obj,"securityPolicyChange", securityPolicyChange);
		field_to_json(Obj,"currentPassword",currentPassword);
		field_to_json(Obj,"lastPasswords",lastPasswords);
		field_to_json(Obj,"oauthType",oauthType);
		field_to_json(Obj,"oauthUserInfo",oauthUserInfo);
    };

    bool UserInfo::from_json(const Poco::JSON::Object::Ptr &Obj) {
        try {
			field_from_json(Obj,"Id",Id);
			field_from_json(Obj,"name",name);
			field_from_json(Obj,"description",description);
			field_from_json(Obj,"avatar",avatar);
			field_from_json(Obj,"email",email);
			field_from_json(Obj,"validationEmail",validationEmail);
			field_from_json(Obj,"validationURI",validationURI);
			field_from_json(Obj,"currentLoginURI",currentLoginURI);
			field_from_json(Obj,"locale",locale);
			field_from_json(Obj,"notes",notes);
			field_from_json(Obj,"userRole",userRole);
			field_from_json(Obj,"securityPolicy",securityPolicy);
			field_from_json(Obj,"userTypeProprietaryInfo",userTypeProprietaryInfo);
			field_from_json(Obj,"validationDate",validationDate);
			field_from_json(Obj,"creationDate",creationDate);
			field_from_json(Obj,"lastLogin",lastLogin);
			field_from_json(Obj,"lastPasswordChange",lastPasswordChange);
			field_from_json(Obj,"lastEmailCheck",lastEmailCheck);
			field_from_json(Obj,"securityPolicyChange",securityPolicyChange);
			field_from_json(Obj,"validated",validated);
			field_from_json(Obj,"changePassword",changePassword);
			field_from_json(Obj,"waitingForEmailCheck",waitingForEmailCheck);
			field_from_json(Obj,"suspended",suspended);
			field_from_json(Obj,"blackListed",blackListed);
			field_from_json(Obj,"currentPassword",currentPassword);
			field_from_json(Obj,"lastPasswords",lastPasswords);
			field_from_json(Obj,"oauthType",oauthType);
			field_from_json(Obj,"oauthUserInfo",oauthUserInfo);
            return true;
        } catch (const Poco::Exception &E) {

        }
        return false;
    };

	void InternalServiceInfo::to_json(Poco::JSON::Object &Obj) const {
		field_to_json(Obj,"privateURI",privateURI);
		field_to_json(Obj,"publicURI",publicURI);
		field_to_json(Obj,"token",token);
	};

	bool InternalServiceInfo::from_json(const Poco::JSON::Object::Ptr &Obj) {
		try {
			field_from_json(Obj,"privateURI",privateURI);
			field_from_json(Obj,"publicURI",publicURI);
			field_from_json(Obj,"token",token);
			return true;
		} catch (...) {

		}
		return false;
	};

	void InternalSystemServices::to_json(Poco::JSON::Object &Obj) const {
		field_to_json(Obj,"key",key);
		field_to_json(Obj,"version",version);
		field_to_json(Obj,"services",services);
	};

	bool InternalSystemServices::from_json(const Poco::JSON::Object::Ptr &Obj)  {
		try {
			field_from_json(Obj, "key", key);
			field_from_json(Obj, "version", version);
			field_from_json(Obj, "services", services);
			return true;
		} catch(...) {

		}
		return false;
	};

	void SystemEndpoint::to_json(Poco::JSON::Object &Obj) const {
		field_to_json(Obj,"type",type);
		field_to_json(Obj,"id",id);
		field_to_json(Obj,"vendor",vendor);
		field_to_json(Obj,"uri",uri);
		field_to_json(Obj,"authenticationType",authenticationType);
	};

	bool SystemEndpoint::from_json(const Poco::JSON::Object::Ptr &Obj) {
		try {
			field_from_json(Obj, "type", type);
			field_from_json(Obj, "id", id);
			field_from_json(Obj, "vendor", vendor);
			field_from_json(Obj, "uri", uri);
			field_from_json(Obj, "authenticationType", authenticationType);
			return true;
		} catch (...) {

		}
		return false;
	};

	void SystemEndpointList::to_json(Poco::JSON::Object &Obj) const {
		field_to_json(Obj,"endpoints",endpoints);
	}

	bool SystemEndpointList::from_json(const Poco::JSON::Object::Ptr &Obj)  {
		try {
			field_from_json(Obj, "endpoints", endpoints);
			return true;
		} catch (...) {

		}
		return false;
	}

	void UserInfoAndPolicy::to_json(Poco::JSON::Object &Obj) const {
		Poco::JSON::Object	UI, TI;
		userinfo.to_json(UI);
		webtoken.to_json(TI);
		Obj.set("tokenInfo",TI);
		Obj.set("userInfo",UI);
	}

	bool UserInfoAndPolicy::from_json(const Poco::JSON::Object::Ptr &Obj) {
		try {
			field_from_json(Obj, "tokenInfo", webtoken);
			field_from_json(Obj, "userInfo", userinfo);
			return true;
		} catch(...) {

		}
		return false;
	}

}
