#pragma once
#include "steam_api.h"
#include "base/Log.h"

#define GAME_ID 1651400
#define DEMO_ID 3399490

#define STEAM_REQUIRED

// Result码
#define STEAM_INIT_FAILED -1
#define STEAM_INIT_SUCCESS 0

// 合法性校验宏
#define STEAM_IF_STEAM_RUNNING // 是否存在正在运行的Steam实例
// #define STEAM_IF_USER_LOGGED // 用户是否已经登陆
// #define STEAM_IF_USER_SUBSCRIBED // 玩家是否持有此游戏，该宏相关的代码需要使用一个无游戏本体但已经登陆的账号来测试

// Steam环境预处理
int SteamPreProcess();

// Steam环境后处理
int SteamPostProcess();