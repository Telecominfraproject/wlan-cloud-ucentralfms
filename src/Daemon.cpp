//
// Created by Stephane Bourque on 2021-05-07.
//

#include <aws/core/Aws.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/AccessControlPolicy.h>
#include <aws/s3/model/PutBucketAclRequest.h>
#include <aws/s3/model/GetBucketAclRequest.h>

#include "Daemon.h"
#include "StorageService.h"
#include "ManifestCreator.h"
#include "NewConnectionHandler.h"
#include "LatestFirmwareCache.h"
#include "DeviceCache.h"
#include "FirmwareCache.h"
#include "AutoUpdater.h"
#include "NewCommandHandler.h"

namespace OpenWifi {
    class Daemon *Daemon::instance_ = nullptr;

    class Daemon *Daemon::instance() {
        if (instance_ == nullptr) {
            instance_ = new Daemon(vDAEMON_PROPERTIES_FILENAME,
                                   vDAEMON_ROOT_ENV_VAR,
                                   vDAEMON_CONFIG_ENV_VAR,
                                   vDAEMON_APP_NAME,
                                   vDAEMON_BUS_TIMER,
                                   SubSystemVec{
                                            StorageService(),
                                            FirmwareCache(),
                                            LatestFirmwareCache(),
                                            DeviceCache(),
                                            NewConnectionHandler(),
                                            ManifestCreator(),
                                            AutoUpdater(),
                                            NewCommandHandler()
                                   });
        }
        return instance_;
    }

    void Daemon::initialize(Poco::Util::Application &self) {
        MicroService::initialize(*this);
    }

    void MicroServicePostInitialization() {

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
