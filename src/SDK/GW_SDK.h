//
// Created by stephane bourque on 2021-10-05.
//

#pragma once

#include <string>

namespace OpenWifi::SDK::GW {

    bool SendFirmwareUpgradeCommand( const std::string & serialNumber, const std::string & URI, uint64_t When = 0 );

};

