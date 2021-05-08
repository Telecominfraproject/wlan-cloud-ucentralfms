//
// Created by stephane bourque on 2021-05-07.
//

#include "RESTAPI_objects.h"
#include "RESTAPI_handler.h"

namespace uCentral::Objects {

    void Firmware::to_json(Poco::JSON::Object &Obj) {

    }

    void Callback::to_json(Poco::JSON::Object &Obj) {

    }

    void AclTemplate::to_json(Poco::JSON::Object &Obj) const {
        Obj.set("Read",Read_);
        Obj.set("ReadWrite",ReadWrite_);
        Obj.set("ReadWriteCreate",ReadWriteCreate_);
        Obj.set("Delete",Delete_);
        Obj.set("PortalLogin",PortalLogin_);
    }

    void WebToken::to_json(Poco::JSON::Object & Obj) const {
        Poco::JSON::Object  AclTemplateObj;
        acl_template_.to_json(AclTemplateObj);
        Obj.set("access_token",access_token_);
        Obj.set("refresh_token",refresh_token_);
        Obj.set("token_type",token_type_);
        Obj.set("expires_in",expires_in_);
        Obj.set("idle_timeout",idle_timeout_);
        Obj.set("created",RESTAPIHandler::to_RFC3339(created_));
        Obj.set("username",username_);
        Obj.set("aclTemplate",AclTemplateObj);
    }

}
