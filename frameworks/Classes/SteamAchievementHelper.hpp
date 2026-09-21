#pragma once
#include "steam_api.h"
#include "base/Log.h"
#include "LogSystem.h"
#include <utility>
#include <string>
#include <vector>
#include <thread>
#include <condition_variable>
#include <mutex>
using SteamAchievementStatus = std::pair<std::string, bool>;

static std::vector<std::string> AchievementList =
{
	"ACH_1ST_ENTER_1_0"
};

namespace lstg
{
	class SteamAchievementHelper
	{
	public:
		SteamAchievementHelper();
		~SteamAchievementHelper();

		static SteamAchievementHelper* getInstance()
		{
			static SteamAchievementHelper instance;
			return &instance;
		};

		std::vector<SteamAchievementStatus> getSteamAchievementList();

		void unlockAchievement(std::string ach_name);

		void resetAchievement(std::string ach_name);

		bool getAchievementStatus(std::string ach_name);

		// 异步向服务器提交成就数据直到StoreStats方法成功
		void SubmitChangeAsync();

		std::string getSteamLanguageSetting();

		bool isDataCached = false;
		bool isSubmitting = false;
		std::mutex submitMutex;
		std::condition_variable submitCondition;
		std::thread submitThread{};
	};
}