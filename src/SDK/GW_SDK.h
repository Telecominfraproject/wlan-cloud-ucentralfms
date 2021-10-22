//
// Created by stephane bourque on 2021-10-05.
//

#ifndef OWFMS_GW_SDK_H
#define OWFMS_GW_SDK_H


#include "framework/MicroService.h"

namespace OpenWifi::SDK::GW {

    bool SendFirmwareUpgradeCommand( const std::string & serialNumber, const std::string & URI, uint64_t When = 0 );

};

#endif //OWFMS_GW_SDK_H
