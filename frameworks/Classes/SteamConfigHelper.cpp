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

uint64 lstg::SteamConfigHelper::getSteamID()
{
	return SteamUser()->GetSteamID().ConvertToUint64();
}

std::string lstg::SteamConfigHelper::getUserName()
{
	return std::string(SteamFriends()->GetPersonaName());
}
