//
// Created by stephane bourque on 2021-10-04.
//

#ifndef OWFMS_PROV_SDK_H
#define OWFMS_PROV_SDK_H

#include "OpenAPIRequest.h"

namespace OpenWifi::SDK::Prov {

    bool GetFirmwareOptions( const std::string & serialNumber, std::string &firmwareUpgrade,
                             bool &firmwareRCOnly);

};


#endif //OWFMS_PROV_SDK_H
