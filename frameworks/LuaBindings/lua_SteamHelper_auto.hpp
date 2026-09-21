#pragma once
#include "base/ccConfig.h"
#include "LuaBindings.h"

extern int luaReg_SteamHelper_lstgSteamHelper(lua_State* lua_S);

inline int luaReg_SteamHelper(lua_State* lua_S)
{
	luaReg_SteamHelper_lstgSteamHelper(lua_S);
	return 0;
}