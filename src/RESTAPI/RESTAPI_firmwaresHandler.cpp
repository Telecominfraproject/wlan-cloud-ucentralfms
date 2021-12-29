//
// Created by stephane bourque on 2021-05-09.
//

#include "RESTAPI_firmwaresHandler.h"
#include "StorageService.h"
#include "LatestFirmwareCache.h"
#include "framework/RESTAPI_protocol.h"

namespace OpenWifi {
    void
    RESTAPI_firmwaresHandler::DoGet() {
        std::string DeviceType = GetParameter(RESTAPI::Protocol::DEVICETYPE, "");
        bool IdOnly = GetBoolParameter(RESTAPI::Protocol::IDONLY, false);
        bool RevisionSet = GetBoolParameter(RESTAPI::Protocol::REVISIONSET, false);
        bool LatestOnly = GetBoolParameter(RESTAPI::Protocol::LATESTONLY, false);
        bool DeviceSet = GetBoolParameter(RESTAPI::Protocol::DEVICESET, false);

        if(DeviceSet) {
            auto Revisions = LatestFirmwareCache()->GetDevices();
            Poco::JSON::Array ObjectArray;
            for (const auto &i:Revisions) {
                ObjectArray.add(i);
            }
            Poco::JSON::Object RetObj;
            RetObj.set(RESTAPI::Protocol::DEVICETYPES, ObjectArray);
            return ReturnObject(RetObj);
        }

        if(RevisionSet) {
            auto Revisions = LatestFirmwareCache()->GetRevisions();
            Poco::JSON::Array ObjectArray;
            for (const auto &i:Revisions) {
                ObjectArray.add(i);
            }
            Poco::JSON::Object RetObj;
            RetObj.set(RESTAPI::Protocol::REVISIONS, ObjectArray);
            return ReturnObject(RetObj);
        }

        // special cases: if latestOnly and deviceType
        if(!DeviceType.empty()) {
            if(LatestOnly) {
                LatestFirmwareCacheEntry    Entry;
                if(!LatestFirmwareCache()->FindLatestFirmware(DeviceType,Entry)) {
                    return NotFound();
                }

                FMSObjects::Firmware    F;
                if(StorageService()->FirmwaresDB().GetFirmware(Entry.Id,F)) {
                    Poco::JSON::Object  Answer;
                    F.to_json(Answer);
                    return ReturnObject(Answer);
                }
                return NotFound();
            } else {
                std::vector<FMSObjects::Firmware> List;
                if (StorageService()->FirmwaresDB().GetFirmwares(QB_.Offset, QB_.Limit, DeviceType, List)) {
                    Poco::JSON::Array ObjectArray;
                    for (const auto &i:List) {
                        if(IdOnly) {
                            ObjectArray.add(i.id);
                        } else {
                            Poco::JSON::Object Obj;
                            i.to_json(Obj);
                            ObjectArray.add(Obj);
                        }
                    }
                    Poco::JSON::Object RetObj;
                    RetObj.set(RESTAPI::Protocol::FIRMWARES, ObjectArray);
                    return ReturnObject(RetObj);
                } else {
                    return NotFound();
                }
            }
        }

        std::vector<FMSObjects::Firmware> List;
        Poco::JSON::Array ObjectArray;
        Poco::JSON::Object Answer;
        if (StorageService()->FirmwaresDB().GetFirmwares(QB_.Offset, QB_.Limit, DeviceType, List)) {
            for (const auto &i:List) {
                if(IdOnly) {
                    ObjectArray.add(i.id);
                } else {
                    Poco::JSON::Object Obj;
                    i.to_json(Obj);
                    ObjectArray.add(Obj);
                }
            }
        }
        Answer.set(RESTAPI::Protocol::FIRMWARES, ObjectArray);
        ReturnObject(Answer);
    }
}
