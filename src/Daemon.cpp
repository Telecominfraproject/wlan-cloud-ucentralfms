//
// Created by Stephane Bourque on 2021-05-07.
//

#include <aws/core/Aws.h>
#include <aws/s3/model/AccessControlPolicy.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/GetBucketAclRequest.h>
#include <aws/s3/model/PutBucketAclRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

#include "AutoUpdater.h"
#include "Daemon.h"
#include "DeviceCache.h"
#include "FirmwareCache.h"
#include "LatestFirmwareCache.h"
#include "ManifestCreator.h"
#include "NewCommandHandler.h"
#include "NewConnectionHandler.h"
#include "StorageService.h"

#include "framework/UI_WebSocketClientServer.h"

namespace OpenWifi {
	class Daemon *Daemon::instance_ = nullptr;

	class Daemon *Daemon::instance() {
		if (instance_ == nullptr) {
			instance_ = new Daemon(
				vDAEMON_PROPERTIES_FILENAME, vDAEMON_ROOT_ENV_VAR, vDAEMON_CONFIG_ENV_VAR,
				vDAEMON_APP_NAME, vDAEMON_BUS_TIMER,
				SubSystemVec{StorageService(), FirmwareCache(), LatestFirmwareCache(),
							 DeviceCache(), NewConnectionHandler(), ManifestCreator(),
							 AutoUpdater(), NewCommandHandler(), UI_WebSocketClientServer()});
		}
		return instance_;
	}

	void Daemon::PostInitialization([[maybe_unused]] Poco::Util::Application &self) {}

	void DaemonPostInitialization(Poco::Util::Application &self) {
		Daemon()->PostInitialization(self);
	}

} // namespace OpenWifi

int main(int argc, char **argv) {
	SSL_library_init();
	Aws::SDKOptions AwsOptions;
	AwsOptions.memoryManagementOptions.memoryManager = nullptr;
	AwsOptions.cryptoOptions.initAndCleanupOpenSSL = false;
	AwsOptions.httpOptions.initAndCleanupCurl = true;

	Aws::InitAPI(AwsOptions);

	int ExitCode = 0;
	{
		auto App = OpenWifi::Daemon::instance();
		ExitCode = App->run(argc, argv);
	}

	ShutdownAPI(AwsOptions);
	return ExitCode;
}
