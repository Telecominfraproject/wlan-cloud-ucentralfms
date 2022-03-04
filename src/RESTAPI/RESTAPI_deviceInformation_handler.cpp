//
// Created by stephane bourque on 2022-03-04.
//

#include "RESTAPI_deviceInformation_handler.h"
#include "StorageService.h"
#include "LatestFirmwareCache.h"

namespace OpenWifi {
    void RESTAPI_deviceInformation_handler::DoGet() {
        auto SerialNumber = GetBinding("serialNumber","");

        if(SerialNumber.empty() || !Utils::ValidSerialNumber(SerialNumber)) {
            return BadRequest(RESTAPI::Errors::MissingOrInvalidParameters);
        }

        FMSObjects::DeviceInformation   DI;

        //  Let's get the history
        StorageService()->HistoryDB().GetHistory(SerialNumber,0,100,DI.history.history);

        // Let's get the DeviceConnectionInformation
        FMSObjects::DeviceConnectionInformation DCI;
        StorageService()->DevicesDB().GetDevice(SerialNumber,DCI);

        LatestFirmwareCacheEntry    LFE;
        LatestFirmwareCache()->FindLatestFirmware(DCI.deviceType,LFE);

        DI.serialNumber = SerialNumber;
        DI.currentFirmware = DCI.revision;
        DI.latestFirmware = LFE.Revision;
        DI.latestFirmwareDate = LFE.TimeStamp;

        FirmwaresDB::RecordName FI;
        StorageService()->FirmwaresDB().GetFirmwareByRevision(DCI.revision,DCI.deviceType,FI);
        DI.currentFirmwareDate = FI.imageDate;

        Poco::JSON::Object  Answer;
        DI.to_json(Answer);
        return ReturnObject(Answer);
    }
}