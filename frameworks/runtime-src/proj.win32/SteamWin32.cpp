#include "SteamWin32.h"
#include "Windows.h"

int SteamPreProcess()
{
#ifdef STEAM_IF_STEAM_RUNNING
	if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
	{
		CC_LOG_ERROR("[Steam] Application is not launch by Steam.");
		MessageBox(NULL, L"Application is not launch by Steam.", L"Error", MB_OK | MB_ICONERROR);
		return STEAM_INIT_FAILED;
	}
#endif

	SteamErrMsg err_msg;
	if (SteamAPI_InitEx(&err_msg) != k_ESteamAPIInitResult_OK)
	{
		const char* msg = err_msg;
		CC_LOG_ERROR("[Steam] Steam API Init Failed.Error:%s", msg);
		MessageBox(NULL, L"Steam API Initialize failed.", L"Error", MB_OK | MB_ICONERROR);
		return STEAM_INIT_FAILED;
	}

	// 校验合法性

#ifdef STEAM_IF_USER_LOGGED
	if ( !SteamUser()->BLoggedOn()) // 是否登录，该API为实时联网校验
	{
		CC_LOG_ERROR("[Steam] Steam account not Logged in.Please check your Steam status.");
		MessageBox(NULL, L"Steam account not Logged in.\nPlease check your Steam status.", L"Error", MB_OK | MB_ICONERROR);
		return STEAM_INIT_FAILED;
	}
#endif

#ifdef STEAM_IF_USER_SUBSCRIBED
	if (!SteamApps()->BIsSubscribedApp(DEMO_ID));
	{
		CC_LOG_ERROR("[Steam] Your Steam account don't subscribed this game.Please support our game!");
		MessageBox(NULL, L"Your Steam account don't subscribed this game.\nPlease support our game!", L"Error", MB_OK | MB_ICONERROR);
		return STEAM_INIT_FAILED;
	}
#endif
	CC_LOG_INFO("[Steam] Steam API initialized.");
	return STEAM_INIT_SUCCESS;
}

int SteamPostProcess()
{
	SteamAPI_Shutdown();
	return 0;
}
