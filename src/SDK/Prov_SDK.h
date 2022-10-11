//
// Created by stephane bourque on 2021-10-04.
//

#pragma once

#include <string>

namespace OpenWifi::SDK::Prov {

    bool GetFirmwareOptions( const std::string & serialNumber, std::string &firmwareUpgrade,
                             bool &firmwareRCOnly);

};

