#include "SteamConfigHelper.hpp"

lstg::SteamConfigHelper* lstg::SteamConfigHelper::getInstance()
{
	static SteamConfigHelper instance;
	return &instance;
}

std::string lstg::SteamConfigHelper::getSteamLanguage()
{
	return std::string(SteamApps()->GetCurrentGameLanguage());
}

std::string lstg::SteamConfigHelper::getSteamID()
{
	return std::to_string(SteamUser()->GetSteamID().ConvertToUint64());
}

std::string lstg::SteamConfigHelper::getUserName()
{
	return std::string(SteamFriends()->GetPersonaName());
}
