//
// Created by stephane bourque on 2021-11-21.
//

#pragma once

#include "framework/OpenWifiTypes.h"
#include "framework/SubSystemServer.h"

namespace OpenWifi {

	class NewCommandHandler : public SubSystemServer, Poco::Runnable {
	  public:
		static auto instance() {
			static auto instance_ = new NewCommandHandler;
			return instance_;
		}

		void run() override;
		int Start() override;
		void Stop() override;
		bool Update();
		void CommandReceived(const std::string &Key, const std::string &Message);

	  private:
		Poco::Thread Worker_;
		std::atomic_bool Running_ = false;
		int WatcherId_ = 0;
		Types::StringPairQueue NewCommands_;

		NewCommandHandler() noexcept
			: SubSystemServer("NewCommandHandler", "NEWCOM-MGR", "commanmdhandler") {}
	};
	inline auto NewCommandHandler() { return NewCommandHandler::instance(); };

} // namespace OpenWifi
