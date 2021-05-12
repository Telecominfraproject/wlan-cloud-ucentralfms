//
// Created by stephane bourque on 2021-05-07.
//

#include "RESTAPI_objects.h"
#include "RESTAPI_handler.h"
#include "Poco/JSON/Parser.h"

namespace uCentral::Objects {

    void EmbedDocument(const std::string & ObjName, Poco::JSON::Object & Obj, const std::string &ObjStr) {
        std::string D = ObjStr.empty() ? "{}" : ObjStr;
        Poco::JSON::Parser P;
        Poco::Dynamic::Var result = P.parse(D);
        const auto &DetailsObj = result.extract<Poco::JSON::Object::Ptr>();
        Obj.set(ObjName, DetailsObj);
    }

    void Firmware::to_json(Poco::JSON::Object &Obj) const {
        Obj.set("uuid",UUID);
        Obj.set("description", Description);
        Obj.set("uploaded",RESTAPIHandler::to_RFC3339(Uploaded));
        Obj.set("firmwareDate",RESTAPIHandler::to_RFC3339(FirmwareDate));
        Obj.set("firmwareVersion", FirmwareVersion);
        Obj.set("firmwareHash", FirmwareHash);
        Obj.set("owner", Owner);
        Obj.set("location", Location);
        Obj.set("downloadCount", DownloadCount);
        Obj.set("uploader", Uploader);
        Obj.set("size", Size);
        Obj.set("digest", Digest);
        Obj.set("s3uri", S3URI);
        Obj.set("deviceType", DeviceType);
        EmbedDocument("firmwareLatestDoc",Obj,FirmwareLatestDoc);
    }

    void Callback::to_json(Poco::JSON::Object &Obj) const {
        Obj.set("uri", URI);
        Obj.set("uuid",UUID);
        Obj.set("location",Location);
        Obj.set("creator", Creator);
        Obj.set("token", Token);
        Obj.set("tokenType", TokenType);
        Obj.set("topics",Topics);
        Obj.set("created",RESTAPIHandler::to_RFC3339(Created));
        Obj.set("expires",RESTAPIHandler::to_RFC3339(Expires));
    }

    bool Callback::from_json(Poco::JSON::Object::Ptr Obj) {
        Poco::DynamicStruct ds = *Obj;

        if(ds.contains("uri"))
            URI = ds["uri"].toString();
        if(ds.contains("uuid"))
            UUID = ds["uuid"].toString();
        if(ds.contains("location"))
            Location = ds["location"].toString();
        if(ds.contains("token"))
            Token = ds["token"].toString();
        if(ds.contains("tokenType"))
            TokenType = ds["tokenType"].toString();

        return true;
    }

    void LatestFirmware::to_json(Poco::JSON::Object &Obj) const {
        Obj.set("deviceType", DeviceType);
        Obj.set("uuid", UUID);
        Obj.set("lastUpdated", RESTAPIHandler::to_RFC3339(LastUpdated));
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
