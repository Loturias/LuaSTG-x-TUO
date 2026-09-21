#pragma once
#include "steam_api.h"
#include <string>

namespace lstg
{
	class SteamConfigHelper
	{
	public:
		static SteamConfigHelper* getInstance();

		std::string getSteamLanguage();

		uint64 getSteamID();

		std::string getUserName();

	private:
		SteamConfigHelper() = default;
		~SteamConfigHelper() = default;
		SteamConfigHelper(const SteamConfigHelper&) = delete;
		SteamConfigHelper& operator=(const SteamConfigHelper&) = delete;
		SteamConfigHelper(SteamConfigHelper&&) = delete;
		SteamConfigHelper& operator=(SteamConfigHelper&&) = delete;
	};
}
