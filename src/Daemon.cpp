//
// Created by Stephane Bourque on 2021-05-07.
//

#include <boost/algorithm/string.hpp>

#include <aws/core/Aws.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/AccessControlPolicy.h>
#include <aws/s3/model/PutBucketAclRequest.h>
#include <aws/s3/model/GetBucketAclRequest.h>


#include "Poco/Util/Application.h"
#include "Poco/Net/SSLManager.h"

#include "Daemon.h"
#include "StorageService.h"
#include "RESTAPI_server.h"
#include "RESTAPI_InternalServer.h"
#include "ManifestCreator.h"
#include "KafkaManager.h"
#include "NewConnectionHandler.h"
#include "LatestFirmwareCache.h"
#include "DeviceCache.h"
#include "FirmwareCache.h"

namespace OpenWifi {
    class Daemon *Daemon::instance_ = nullptr;

    class Daemon *Daemon::instance() {
        if (instance_ == nullptr) {
            instance_ = new Daemon(vDAEMON_PROPERTIES_FILENAME,
                                   vDAEMON_ROOT_ENV_VAR,
                                   vDAEMON_CONFIG_ENV_VAR,
                                   vDAEMON_APP_NAME,
                                   vDAEMON_BUS_TIMER,
                                   Types::SubSystemVec{Storage(),
                                                       FirmwareCache(),
                                                       LatestFirmwareCache(),
                                                       DeviceCache(),
                                                       NewConnectionHandler(),
                                                       RESTAPI_server(),
                                                       RESTAPI_InternalServer(),
                                                       ManifestCreator()
                                   });
        }
        return instance_;
    }

    void Daemon::initialize(Poco::Util::Application &self) {
        MicroService::initialize(*this);
    }

}

int main(int argc, char **argv) {
    SSL_library_init();
    Aws::SDKOptions AwsOptions;
    AwsOptions.memoryManagementOptions.memoryManager = nullptr;
    AwsOptions.cryptoOptions.initAndCleanupOpenSSL = false;
    AwsOptions.httpOptions.initAndCleanupCurl = true;

    Aws::InitAPI(AwsOptions);

    int ExitCode=0;
    {
        auto App = OpenWifi::Daemon::instance();
        ExitCode = App->run(argc, argv);
    }

    ShutdownAPI(AwsOptions);
    return ExitCode;
}
